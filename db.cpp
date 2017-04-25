
#include "Common.h"
#include "BPlusTree.h"
#include "Configuration.h"
#include "QueryFilter.h"
#include "SQLSession.h"
#include "Models/ColumnModel.h"
#include "Models/IndexModel.h"
#include "Models/TableModel.h"
#include "Models/DataBaseModel.h"
#include "Scanners/QueryScanner.h"
#include "Scanners/LinearQueryScanner.h"
#include "Scanners/RangeIndexQueryScanner.h"
#include "Scanners/OneIndexQueryScanner.h"
#include "Services/FileService.h"
#include "Services/BlockService.h"
#include "Services/MetaDataService.h"
#include "Services/RecordService.h"
#include "QueryPlans/QueryPlan.h"
#include "QueryPlans/SelectQueryPlan.h"
#include "QueryPlans/InsertQueryPlan.h"
#include "QueryPlans/SQLCondition.h"
#include "QueryPlans/SQLWhereClause.h"
#include "QueryPlans/CreateDataBaseQueryPlan.h"
#include "QueryPlans/CreateTableQueryPlan.h"

using namespace std;


class SQLParser{
    SQLSession * sqlSession;
    const char * str = nullptr;// = "create table test2( id int, value char(20), primary key(id) );";
    //const char * str = "insert into test (id, value) values (20, '20');";
    //const char * str = "select id, value from test where value = '20' and value = 'F';";
public:
    SQLParser(SQLSession *sqlSession) : sqlSession(sqlSession) {}


    enum class TokenType {
        integer, real, name, string, ope, null, __keyword
    };
    struct Token {
        int begin, end;
        TokenType type;
        Token(int begin_,int end_, TokenType type_)
                :begin(begin_),end(end_),type(type_){}
    };
    std::vector<Token> tokens;
    size_t pos = 0;

    inline bool isNum(char x) {
        return x >= '0' && x <= '9';
    }
    inline bool isChar(char x) {
        return (x >= 'a' && x <= 'z') || (x >= 'A' && x <='Z') || x == '_' || x == '-';
    }
    void tokenize() {
        TokenType status = TokenType::null;
        int saved_pos = 0;
        for (int i = 0; str[i] != 0; i++) {
            switch (status) {
                case TokenType::null:
                    if (str[i] == '\'' || str[i] == '\"') {
                        status = TokenType::string;
                    }
                    else if (str[i] == '`') {
                        status = TokenType::name;
                    }
                    else if (isNum(str[i]) || str[i] == '-') {
                        status = TokenType::integer;
                    }
                    else if (isChar(str[i])) {
                        status = TokenType::__keyword;
                    }
                    else if (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\r') {
                        break;
                    }
                    else {
                        int st = i;
                        if (str[i] == '!' && str[i + 1] == '=') {
                            i++;
                        }
                        else if (str[i] == '<' && str[i + 1] == '>') {
                            i++;
                        }
                        else if (str[i] == '<' && str[i + 1] == '=') {
                            i++;
                        }
                        else if (str[i] == '>' && str[i + 1] == '=') {
                            i++;
                        }
                        tokens.emplace_back(st, i, TokenType::ope);
                        break;
                    }
                    saved_pos = i;
                    break;
                case TokenType::string:
                    if ((str[i] == '\"' || str[i] == '\'') && str[i - 1] != '\\') {
                        tokens.emplace_back(saved_pos + 1, i - 1, TokenType::string);
                        status = TokenType::null;
                    }break;
                case TokenType::name:
                    if (str[i] == '`'  && str[i - 1] != '\\') {
                        tokens.emplace_back(saved_pos, i - 1, TokenType::string);
                        status = TokenType::null;
                    }
                    break;
                case TokenType::integer:
                    if (str[i] == '.' || str[i] == 'e') {
                        status = TokenType::real;
                        if (str[i] == 'e' && str[i] == '-')i++;
                        break;
                    }
                    if (!isNum(str[i])) {
                        tokens.emplace_back(saved_pos, i - 1, TokenType::integer);
                        status = TokenType::null;
                        i--;
                    }
                    break;
                case TokenType::real:
                    if (!isNum(str[i])) {
                        tokens.emplace_back(saved_pos, i - 1, TokenType::real);
                        status = TokenType::null;
                        i--;
                    }
                    break;
                case TokenType::__keyword:
                    if (!isChar(str[i]) && !isNum(str[i])) {
                        tokens.emplace_back(saved_pos, i - 1, TokenType::name);
                        status = TokenType::null;
                        i--;
                    }
                    break;
                case TokenType::ope:break;
            }
        }
    }


    bool tokencmp(Token & token, const char * target){
        int len1 = token.end - token.begin + 1;
        int len2 = (int)strlen(target);
        if( len1 != len2 ) return false;
        return strncasecmp(str + token.begin, target, (size_t)len1) == 0;
    }

    std::list<SQLCondition> * parseWhereClause(TableModel * tableModel){
        Token token = tokens[pos];
        auto conditionList = new std::list<SQLCondition>;
        try {
            while (token.type == TokenType::name) {
                int cid = tableModel->getColumnIndex(std::string(str, token.begin, token.end - token.begin + 1));
                SQLCondition::Type type;
                pos ++; token = tokens[pos]; // col
                if(tokencmp(token, "<")){
                    type = SQLCondition::Type::lt;
                }
                else if(tokencmp(token, ">")){
                    type = SQLCondition::Type::gt;
                }
                else if(tokencmp(token, ">=")){
                    type = SQLCondition::Type::gte;
                }
                else if(tokencmp(token, "<=")){
                    type = SQLCondition::Type::lte;
                }
                else if(tokencmp(token, "<>")){
                    type = SQLCondition::Type::neq;
                }
                else if(tokencmp(token, "=")){
                    type = SQLCondition::Type::eq;
                }
                else throw new SQLSyntaxException(0, "illegal opes");
                pos ++; token = tokens[pos]; // op
                void * data;
                ColumnModel * columnModel = tableModel->getColumn(cid);
                if(token.type == TokenType::integer){
                    if( columnModel->getType() != ColumnType::Int ) throw SQLTypeException(3, "type error");
                    data = new int[1];
                    sscanf(str + token.begin, "%d", (int *)data);
                }
                else if(token.type == TokenType::string){
                    if( columnModel->getType() != ColumnType::Char ) throw SQLTypeException(3, "type error");
                    size_t len = columnModel->getSize();
                    data = new char[len + 1];
                    memcpy(data, str + token.begin, token.end - token.begin + 1);
                    ((char *)data)[token.end - token.begin + 1] = 0;
                }
                else if(token.type == TokenType::real){
                    if( columnModel->getType() != ColumnType::Float ) throw SQLTypeException(3, "type error");
                    data = new double[1];
                    sscanf(str + token.begin, "%lf", (double *)data);
                }
                else throw new SQLSyntaxException(0, "illegal operand");
                conditionList->emplace_back(type, cid, data);
                pos ++; token = tokens[pos]; // imm
                if(!tokencmp(token, "and")) break;
                pos ++; token = tokens[pos]; // and
            }
            return conditionList;
        }catch(SQLException e){
            delete conditionList;
            throw e;
        }
    }

    std::pair<QueryScanner *, SQLWhereClause *>
    getQueryScanner(TableModel * table, std::list<SQLCondition> * list){
        int status = 0;
        SQLCondition *l = nullptr, *r = nullptr;
        IndexModel *sidx;
        // 0 => no 1 => findone  2=> left 3=>right 4=>findtwo
        SQLWhereClause * where = new SQLWhereClause();
        QueryScanner * scanner = nullptr;
        for(auto & item : *list){
            if(status == 1 || status == 4){
                where->addCondition(new SQLCondition(item.type, item.cid, item.value));
                continue;
            }
            IndexModel * idx = table->findIndexOn(item.cid);
            if(status == 0){
                if(idx != nullptr){
                    if(item.type == SQLCondition::Type::gt
                       ||item.type == SQLCondition::Type::gte){
                        l = &item;
                        status = 2;
                        sidx = idx;
                        continue;
                    }
                    if(item.type == SQLCondition::Type::lt
                       ||item.type == SQLCondition::Type::lte){
                        r = &item;
                        status = 3;
                        sidx = idx;
                        continue;
                    }
                    if(item.type == SQLCondition::Type::eq){
                        l = &item;
                        status = 1;
                        sidx = idx;
                        continue;
                    }
                }
            }
            else if(status == 2
                    && idx == sidx
                    && (item.type == SQLCondition::Type::lt
                        ||item.type == SQLCondition::Type::lte)){
                r = &item;
                status = 4;
                continue;
            }
            else  if(status == 3
                     && idx == sidx
                     && (item.type == SQLCondition::Type::gt
                         ||item.type == SQLCondition::Type::gte)) {
                l = &item;
                status = 4;
                continue;
            }
            where->addCondition(new SQLCondition(item.type, item.cid, item.value));

        }
        if(status == 1){
            scanner = new OneIndexQueryScanner(
                    sidx->getIndexRunner(),
                    sqlSession->getRecordService(),
                    table->getLen(),
                    table->getFid(),
                    l->value,
                    table->getColumn(l->cid)->getName()
            );
        }
        else if(status == 2 ||status == 3 || status == 4){
            scanner = new RangeIndexQueryScanner(
                    sidx->getIndexRunner(),
                    sqlSession->getRecordService(),
                    table->getLen(),
                    table->getFid(),
                    l == nullptr ? l : l->value,
                    r == nullptr ? r : r->value,
                    l == nullptr ? true : l->type == SQLCondition::Type::gte,
                    r == nullptr ? true : r->type == SQLCondition::Type::lte,
                    l != nullptr,
                    r != nullptr,
                    table->getColumn(l->cid)->getName()
            );
        }
        // === select index ===
        delete list;
        return std::make_pair(scanner, where);
    }


    SelectQueryPlan * parseSelectStatement(){
        Token token = tokens[pos];
        std::vector<std::string> columns;
        bool ok = false;
        if(str[token.begin] == '*'){
            pos ++; token = tokens[pos];
            if(tokencmp(token, "from")){ok = true;}
            else throw SQLSyntaxException(2, "syntax error");
        }
        else{
            while(token.type == TokenType::name){
                columns.emplace_back(str, token.begin, token.end - token.begin + 1);
                pos ++; token = tokens[pos];
                if(tokencmp(token, "FROM")){ok = true; break;}
                if(str[token.begin] != ',') throw SQLSyntaxException(2, "syntax error");
                pos ++; token = tokens[pos];
            }
            if(!ok)throw SQLSyntaxException(2, "syntax error");
        }
        pos ++; token = tokens[pos];//FROM
        TableModel * table = sqlSession->getTable(std::string(str, token.begin, token.end - token.begin + 1));
        SQLWhereClause * where = nullptr;
        QueryScanner * scanner = nullptr;
        pos ++; token = tokens[pos];//tableName
        while(str[token.begin] != ';') {
            if (tokencmp(token, "WHERE")) {
                pos ++;
                auto list = parseWhereClause(table);
                auto tmp = getQueryScanner(table, list);
                scanner = tmp.first;
                where = tmp.second;
            } else if (tokencmp(token, "ORDER")) {

            } else {
                if (where != nullptr) delete where;
                throw SQLSyntaxException(2, "syntax error");
            }
            token = tokens[pos];
        }
        if(scanner == nullptr)scanner = new LinearQueryScanner(
                    sqlSession->getRecordService(),
                    table->getFid(),
                    table->getLen());
        return new SelectQueryPlan(table, scanner, columns, where);
    }

    InsertQueryPlan * parseInsertStatement(){
        //auto insertQueryPlan = new InsertQueryPlan();
        Token token = tokens[pos];
        std::vector<size_t> columns;
        if(!tokencmp(token, "into")) throw SQLSyntaxException(0, "syntax error");
        pos++;token = tokens[pos]; //into
        TableModel * table = sqlSession->getTable(std::string(str, token.begin, token.end - token.begin + 1));
        if(table == nullptr)throw SQLExecuteException(0, "no table");
        pos++;token = tokens[pos]; //name
        if(!tokencmp(token, "("))throw SQLSyntaxException(0, "syntax error");
        pos++;token = tokens[pos];// (
        while( pos < tokens.size()){
            columns.emplace_back(table->getColumnIndex(std::string(str, token.begin, token.end - token.begin + 1)));
            pos ++; token = tokens[pos]; // name;
            if(str[token.begin] == ')') break;
            if(str[token.begin] != ',') throw SQLSyntaxException(2, "syntax error");
            pos ++; token = tokens[pos]; // ,
        }
        pos ++; token = tokens[pos]; // )
        if(!tokencmp(token, "values")) throw SQLSyntaxException(2, "syntax error");
        pos ++; token = tokens[pos]; // values
        if(str[token.begin] != '(') throw SQLSyntaxException(2, "syntax error");
        pos ++; token = tokens[pos]; // )
        auto result = new InsertQueryPlan(table);
        int cur = 0;
        char * data = (char *)result->getData();
        char * sdata = (char *)result->getData();
        while( pos < tokens.size()) {
            auto columnModel = table->getColumn(columns[cur]);
            if(token.type == TokenType::integer){
                if( columnModel->getType() != ColumnType::Int ) throw SQLTypeException(3, "type error");
                sscanf(str + token.begin, "%d", (int *)data);
                data += 4;
            }
            else if(token.type == TokenType::string){
                if( columnModel->getType() != ColumnType::Char ) throw SQLTypeException(3, "type error");
                size_t len = columnModel->getSize();
                int real_len = min(token.end - token.begin + 1, (int)(len - 1));
                memcpy(data, str + token.begin, real_len);
                data[real_len] = 0;
                data += len;
            }
            else if(token.type == TokenType::real){
                if( columnModel->getType() != ColumnType::Float ) throw SQLTypeException(3, "type error");
                sscanf(str + token.begin, "%lf", (double *)data);
                data += 8;
            }
            else  throw SQLSyntaxException(2, "syntax error");
            pos ++; token = tokens[pos]; // value;
            if(str[token.begin] == ')')break;
            if(str[token.begin] != ',') throw SQLSyntaxException(2, "syntax error");
            pos ++; token = tokens[pos]; // ,
            cur ++;
        }
        printf("=====\n");
        for(int i=0;i<=10;i++)putchar(sdata[i]);
        pos ++; token = tokens[pos]; // )
        if(str[token.begin] != ';') throw SQLSyntaxException(2, "syntax error");
        pos ++; token = tokens[pos]; // ;
        return result;
    }

    void parseCreateDefinition(JSONArray * defJson, JSONObject * indexJson){
        auto json = new JSONObject();
        Token token = tokens[pos]; pos ++;
        // name;
        if( token.type != TokenType::name )throw SQLSyntaxException(2, "syntax error");
        if( tokencmp(token, "primary") ){
            token = tokens[pos]; pos ++; //primary
            if( !tokencmp(token, "key"))throw SQLSyntaxException(2, "syntax error");
            token = tokens[pos]; pos ++; //key
            if(str[token.begin] != '(') throw SQLSyntaxException(2, "syntax error");
            token = tokens[pos]; pos ++; // (
            std::string name(str, token.begin, token.end - token.begin + 1);
            token = tokens[pos]; pos ++; // name
            if(str[token.begin] != ')') throw SQLSyntaxException(2, "syntax error");
            json->set("on", name);
            json->set("type", "bplus");
            indexJson->set("primary", json);
            return;
        }
        std::string name(str, token.begin, token.end - token.begin + 1);
        json->set("name", name);
        token = tokens[pos]; pos ++; //name
        if( tokencmp(token, "int")){
            json->set("type", "int");
            json->set("size", 4);
        }
        else if( tokencmp(token, "float")){
            json->set("type", "float");
            json->set("size", 8);
        }
        else if( tokencmp(token, "char")){
            token = tokens[pos]; pos ++; // (
            if(str[token.begin] != '(') throw SQLSyntaxException(2, "syntax error");
            token = tokens[pos]; pos ++; // n
            if(token.type != TokenType::integer)throw SQLSyntaxException(2, "syntax error");
            int n = 0;
            sscanf(str + token.begin, "%d", &n);
            token = tokens[pos]; pos ++; // )
            if(str[token.begin] != ')') throw SQLSyntaxException(2, "syntax error");
            json->set("type", "char");
            json->set("size", n);
        }
        else  throw SQLSyntaxException(2, "syntax error");
        defJson->put(json);
    }

    CreateQueryPlan * parseCreateStatement(){
        Token token = tokens[pos]; pos ++;
        if( tokencmp(token, "table") ){
            token = tokens[pos]; pos ++;// table
            if(token.type != TokenType::name)throw SQLSyntaxException(2, "syntax error");
            std::string name(str, token.begin, token.end - token.begin + 1);
            token = tokens[pos]; pos ++;// name
            if(str[token.begin] != '(') throw SQLSyntaxException(2, "syntax error");
            auto indexJson = new JSONObject();
            auto defJson = new JSONArray();
            while( pos < tokens.size() ){
                parseCreateDefinition(defJson, indexJson);
                token = tokens[pos]; pos ++;
                if(str[token.begin] == ')') break;
                if(str[token.begin] != ',') throw SQLSyntaxException(2, "syntax error");
            }
            token = tokens[pos]; pos ++;// ;
            if(str[token.begin] != ';') throw SQLSyntaxException(2, "syntax error");
            return new CreateTableQueryPlan(name, sqlSession, defJson, indexJson);
        }
        else if( tokencmp(token, "database") ){
            token = tokens[pos]; pos ++;// database;
            if(token.type != TokenType::name)throw SQLSyntaxException(2, "syntax error");
            std::string name(str, token.begin, token.end - token.begin + 1);
            token = tokens[pos]; pos ++;// ;
            if(str[token.begin] != ';') throw SQLSyntaxException(2, "syntax error");
            return new CreateDataBaseQueryPlan(name, sqlSession);
        }
        throw SQLSyntaxException(2, "syntax error");
    }

    QueryPlan * parseSQLStatement(const char * str){
        this->str = str;
        tokens.clear();
        this->tokenize();
        pos = 0;
        int len = tokens[pos].end - tokens[pos].begin + 1;
        Token token = tokens[pos];pos++;
        if( len == 6){
            if(tokencmp(token, "select")){
                if( sqlSession->getDataBaseModel() == nullptr)throw SQLExecuteException(0, "no database selected");
                return parseSelectStatement();
            }
            else if( tokencmp(token, "delete")){
                //TODO:: delete
            }
            else if( tokencmp(token, "insert")){
                if( sqlSession->getDataBaseModel() == nullptr)throw SQLExecuteException(0, "no database selected");
                return parseInsertStatement();
            }
            else if( tokencmp(token, "create")){
                return parseCreateStatement();
            }
            else{
                throw SQLSyntaxException(0, "keyword error");
            }
        }
        else{
            throw SQLSyntaxException(0, "keyword error");
        }
        return nullptr;
    }

    void test(){
        for(auto & x: tokens){
            for(int i = x.begin; i<= x.end; i++){
                putchar(str[i]);
            }
            printf("%d\n", x.end - x.begin + 1);
        }
    }

};


/*
 * a>=35 && a<=35
 * SQL => QueryPlan
 * QueryPlan
 * type => select
 * front => linear / index
 *      index: { on : "primiary", "
 *
 *
 */

class ApplicationContainer{
public:
    BlockService * blockService;
    FileService * fileService;
    MetaDataService * metaDataService;
    RecordService * recordService;
    ApplicationContainer(){}
    ~ApplicationContainer(){
        delete blockService;
        delete fileService;
        delete metaDataService;
        delete recordService;
    }
    void start(){
        // The bootstrap persedure of IoC Container:
        // Initial Configuration Singleton
        // Load FileService
        // Load MetaDataService { Register File => FileService }
        // Load BlockService { Inject FileService }
        // Load QueryRunnerService{ Inject BlockService }
        // Load QueryBuilderService { Inject QueryRunnerService }
        // Load SQLInterpreterService { Inject QueryBuilderSerice }
        Configuration::initialize();
        fileService = new FileService();
        blockService = new BlockService(fileService);
        recordService = new RecordService(blockService);
        metaDataService = new MetaDataService(fileService, blockService);
    }

    void testSQLBatch(){
        auto session = new SQLSession(metaDataService, nullptr, recordService, blockService);
        auto parser = new SQLParser(session);
        char x[256];
        session->loadTable("fuck");
        while(true){
            cin.getline(x, 210);
            if(x[0] == '@')break;
            auto sql = parser->parseSQLStatement(x);
            std::cout<<sql->toJSON()->toString(true)<<endl;
            std::cout<<sql->runQuery(recordService)->toString(true)<<endl;
        }
    }

    void testSQL(){
        auto session = new SQLSession(metaDataService, nullptr, recordService, blockService);
        session->loadTable("test");
        auto parser = new SQLParser(session);
        parser->tokenize();
        parser->test();
        //auto sql = parser->parseSQLStatement();
//        std::cout<<sql->toJSON()->toString(true)<<endl;
//        std::cout<<sql->runQuery(recordService)->toString(true)<<endl;
//        std::cout<<metaDataService->toJSON()->toString(true)<<endl;
    }

#define __fo(x) ((x) / BLOCK_SIZE )
#define __bo(x) ((x) % BLOCK_SIZE )

    void testRecord(int fid){
        char buffer[BLOCK_SIZE];
        buffer[0] = 'a';
        buffer[1] = 'v';
        buffer[2] = 0;
        auto a = recordService->insert(fid, buffer, 8);
        buffer[1] = '1';
        auto b = recordService->insert(fid, buffer, 8);
        buffer[1] = '2';
        auto c = recordService->insert(fid, buffer, 8);
        buffer[1] = '3';
        auto d = recordService->insert(fid, buffer, 8);
        printf("read from record 0 %s\n", recordService->read(fid, __fo(a),__bo(a), 8));
        printf("read from record 1 %s\n", recordService->read(fid, __fo(b),__bo(b), 8));
        printf("read from record 2 %s\n", recordService->read(fid, __fo(c),__bo(c), 8));
        printf("read from record 3 %s\n", recordService->read(fid, __fo(d),__bo(d), 8));
        recordService->write(fid, 0, 8, buffer, 8);
        //recordFacade->remove(fid, 16, 8);
        for(int i = 0; i<= 50; i++){
            printf("read from record %d %s\n",
                   i,
                   recordService->read(fid,__fo(i * 8), __bo(i * 8), 8));
        }
    }
    void testFile(){
        int fid = fileService->openFile("aes.jb");
        printf("open file: fid = %d\n", fid);
        size_t offset = fileService->allocBlock(fid);
        printf("alloc block at offset = %d\n", (int) offset);
        char * data = new char[BLOCK_SIZE];
        data[0] = 'a';
        data[1] = 'b';
        data[2] = 0;
        fileService->writeBlock(fid, offset, (void *)data);
        printf("write block with str = %s\n", data);
        char * out = (char *)fileService->readBlock(fid, offset);
        printf("read block with str = %s\n", out);
        delete[] out;
        delete[] data;
    }
    void testBlockSr(int fid){
        size_t offset = blockService->allocBlock(fid);
        BlockItem * blk = blockService->getBlock(fid, offset);
        char * data = (char * )blk->value;
        data[0] = 'a';
        data[1] = 'b';
        blk->modified = 1;
        blk = blockService->getBlock(fid, offset);
        blk = blockService->getBlock(fid, offset);
        blk = blockService->getBlock(fid, offset);
        blk = blockService->getBlock(fid, offset);
        blk = blockService->getBlock(fid, offset);

    }
    void testBlock(){
        int fid = fileService->openFile("aes.jb");
        for(int i=1;i<=20; i++)testBlockSr(fid);
    }

    void testQueryScanner(int fid){
        char buffer[BLOCK_SIZE];
        size_t len = 8;
        buffer[0] = 'a';
        buffer[1] = 'v';
        buffer[2] = 0;
        auto a = recordService->insert(fid, buffer, len);
        buffer[1] = '1';
        auto b = recordService->insert(fid, buffer, len);
        buffer[1] = '2';
        auto c = recordService->insert(fid, buffer, len);
        buffer[1] = '3';
        auto d = recordService->insert(fid, buffer, len);
        printf("read from record 0 %s\n", recordService->read(fid, __fo(a),__bo(a), 8));
        printf("read from record 1 %s\n", recordService->read(fid, __fo(b),__bo(b), 8));
        printf("read from record 2 %s\n", recordService->read(fid, __fo(c),__bo(c), 8));
        printf("read from record 3 %s\n", recordService->read(fid, __fo(d),__bo(d), 8));
        LinearQueryScanner * linearQueryScanner = new LinearQueryScanner(recordService, fid,len);
        linearQueryScanner->scan([](size_t id, void * ptr){
            printf("scan %lld => %s\n", (long long)id, ptr);
        });
    }
};

//const int NODE_SIZE = 5;
//const int NODE_SIZE_HALF = (NODE_SIZE + 1) / 2; // 2
// 4 bytes (int) 4 bytes(float) 4 bytes char???fucking fuck
// NODE_SIZE_EQU ::=
// NODE_SIZE = (BLOCK_SIZE - 8 ) / ( key_type_size );
// 2 bytes size + 2 bytes isLeaf
// 8 bytes int, 8 bytes long, 16 bytes char, 64 bytes, 256 bytes
#include <sstream>
const char * itos(int i){
    char * buffer = new char[36];
    ostringstream os;
    os << i;
    strcpy(buffer, os.str().c_str());
    return buffer;
}





//BPlusTree<char[16]> bPlusTree;
//int main(){
//    bPlusTree.T_BPLUS_TEST();
//    return 0;
//}
//
//
//int main2(){
//    tokenize();
//    for(auto & x : tokens){
//        for(int i = x.begin; i <= x.end; i++)putchar(str[i]);
//        printf("\n");
//    }
//    return 0;
//}

int main(){
    ApplicationContainer applicationContainer;
    applicationContainer.start();
    applicationContainer.testSQLBatch();
    int fid = applicationContainer.fileService->openFile("test.idx");
//    BPlusTree<char[16]> bPlusTree(applicationContainer.blockService, fid);
//    bPlusTree.T_BPLUS_TEST();
    //applicationContainer.testBlock();
   // applicationContainer.testQueryScanner(fid);
    return 0;
}

/** 

    metablock datablock

**/ 