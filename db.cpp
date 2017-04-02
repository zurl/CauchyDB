
#include "Common.h"
#include "Services/FileService.h"
#include "Services/BlockService.h"
#include "BPlusTree.h"
#include "Configuration.h"
#include "Services/RecordService.h"
#include "QueryFilter.h"
#include "Scanners/QueryScanner.h"
#include "Scanners/LinearQueryScanner.h"
#include "Models/ColumnModel.h"
#include "Models/IndexModel.h"
#include "Models/TableModel.h"
#include "Scanners/RangeIndexQueryScanner.h"
#include "Scanners/OneIndexQueryScanner.h"

using namespace std;



// int char float


class DataBaseModel{
    std::map<std::string, TableModel * > tables;
    public:
    DataBaseModel(FileService * fileService, const std::string & name, JSON * config){
        JSONObject * data = config->get("tables")->toObject();
        for(auto & table: data->hashMap){
            tables.emplace(table.first,
             new TableModel(fileService, name + "_" + table.first, table.second));
        }
    }
    ~DataBaseModel(){
        for( auto & x : tables){
            delete x.second;
        }
    }
    TableModel * getTableByName(const std::string & str){
        return tables[str];
    }
};

class MetaDataService{
    FileService * fileService;
    JSON * data;
    std::map<std::string, DataBaseModel *> dataBases;
public:
    ~MetaDataService(){
        for( auto & x: dataBases){
            delete x.second;
        }
    }
    DataBaseModel * getDataBase(const string & name){
        return dataBases[name];
    }
    MetaDataService(FileService * fileService)
        :fileService(fileService){
        data = JSON::fromFile(Configuration::attrCString("meta_file_path"));
        // Load All DataBases
        JSONObject * dbs = data->get("databases")->toObject();
        for(auto & db: dbs->hashMap){
            dataBases.emplace(db.first, new DataBaseModel(fileService, db.first, db.second));
        }
    }
};
// [nullptr][ ][ ][ ][ ]
// 0->full

// 0 - 7 is header!!


class SQLSession{
    MetaDataService * metaDataService;
    DataBaseModel * dataBaseModel;
    RecordService * recordService;
    BlockService * blockService;
public:
    SQLSession(MetaDataService *metaDataService, DataBaseModel *dataBaseModel, RecordService *recordService,
               BlockService *blockService) : metaDataService(metaDataService), dataBaseModel(dataBaseModel),
                                             recordService(recordService), blockService(blockService) {}

    RecordService *getRecordService() const {
        return recordService;
    }

    BlockService *getBlockService() const {
        return blockService;
    }

    void loadTable(const string & name){
        dataBaseModel = metaDataService->getDataBase(name);
    }

    TableModel * getTable(const string & name){
        if( dataBaseModel == nullptr) return nullptr;
        return dataBaseModel->getTableByName(name);
    }

};



class SQLCondition{
public:
    enum class Type{
        gt, lt, gte, lte, eq, neq
    };
    const char * TypeName[6] = {
            "gt", "lt", "gte", "lte", "eq", "neq"
    };
    Type type;
    int cid;
    void * value;
    SQLCondition(Type type, int cid, void *value) : type(type), cid(cid), value(value) {}
    JSON * toJSON(TableModel * tableModel){
        auto json = new JSONObject();
        json->hashMap.emplace("type", new JSONString(TypeName[ static_cast<unsigned>(type)]));
        json->hashMap.emplace("cid", new JSONString(tableModel->getColumn(cid)->getName()));
        json->hashMap.emplace("value", ColumnTypeUtil::toJSON(tableModel->getColumn(cid)->getType(), value));
        return json;
    }
};

class SQLWhereClause{
    std::vector<SQLCondition *> conds;
public:
    void addCondition(SQLCondition * cond){
        conds.emplace_back(cond);
    }
    JSON * toJSON(TableModel * tableModel){
        auto json = new JSONArray();
        for(auto * x: conds){
            json->elements.emplace_back(x->toJSON(tableModel));
        }
        return json;
    }
};


class QueryPlan{
public:
    virtual JSON * toJSON() = 0;
    virtual JSON * runWithJSON(
            RecordService * recordService,
            BlockService * blockService
    ) = 0;
};

class InsertQueryPlan : public QueryPlan{
    TableModel * tableModel;
    void * data;
public:
    InsertQueryPlan(TableModel *tableModel) : tableModel(tableModel), data(new char[tableModel->getLen()]){}
    ~InsertQueryPlan(){
        delete[] data;
    }

    void *getData() const {
        return data;
    }

    JSON *runWithJSON( RecordService * recordService,
                       BlockService * blockService) override {
        // insert records
        size_t recordId = recordService->insert(tableModel->getFid(), data, tableModel->getLen());

        return nullptr;
    }

    JSON *toJSON() override {
        auto json = new JSONObject();
        json->hashMap.emplace("type", new JSONString("insert"));
        json->hashMap.emplace("table", new JSONString(tableModel->getName()));
        auto jarr = new JSONArray();
        char * cur = (char *)data;
        for(int i = 0; i < tableModel->getColumns().size(); i++){
            jarr->elements.emplace_back(ColumnTypeUtil::toJSON(
                    tableModel->getColumn(i)->getType(),
                    cur
            ));
            cur += tableModel->getColumn(i)->getSize();
        }
        json->hashMap.emplace("value", jarr);
        return json;
    }
};

class SelectQueryPlan : public QueryPlan{
    QueryScanner * queryScanner;
    TableModel * tableModel;
    std::vector<size_t> columns;
    SQLWhereClause * where;
public:
    SelectQueryPlan(TableModel * tableModel,
                    QueryScanner *queryScanner,
                    const vector<std::string> &columns,
                    SQLWhereClause *where)
            : tableModel(tableModel), queryScanner(queryScanner), where(where) {
        for(auto & key : columns){
            this->columns.emplace_back(tableModel->getColumnIndex(key));
        }
    }

    JSON *runWithJSON(RecordService *recordService, BlockService *blockService) override {
        return nullptr;
    }

    ~SelectQueryPlan(){
        delete queryScanner;
        delete where;
    }
private:
    JSON *toJSON() override {
        auto json = new JSONObject();
        auto col = new JSONArray();
        for(auto & x: columns){
            col->elements.emplace_back(new JSONInteger(x));
        }
        json->hashMap.emplace("type", new JSONString("select"));
        json->hashMap.emplace("table", new JSONString(tableModel->getName()));
        json->hashMap.emplace("scanner", queryScanner->toJSON());
        json->hashMap.emplace("columns", col);
        json->hashMap.emplace("where", where == nullptr ? new JSONNull() : where->toJSON(tableModel));
        return json;
    }
};

class SQLParser{
    SQLSession * sqlSession;
    const char * str = "insert into test (id, value) values (20, '20');";
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
        return (x >= 'a' && x <= 'z') || (x >= 'A' && x <='Z');
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
                    if (!isChar(str[i])) {
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
                    sscanf(str + token.begin, "%d", data);
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
                    sscanf(str + token.begin, "%lf", data);
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
                sscanf(str + token.begin, "%lf", data);
                data += 4;
            }
            else  throw SQLSyntaxException(2, "syntax error");
            pos ++; token = tokens[pos]; // value;
            if(str[token.begin] == ')')break;
            if(str[token.begin] != ',') throw SQLSyntaxException(2, "syntax error");
            pos ++; token = tokens[pos]; // ,
            cur ++;
        }
        pos ++; token = tokens[pos]; // )
        if(str[token.begin] != ';') throw SQLSyntaxException(2, "syntax error");
        pos ++; token = tokens[pos]; // ;
        return result;
    }

    QueryPlan * parseSQLStatement(){
        pos = 0;
        int len = tokens[pos].end - tokens[pos].begin + 1;
        Token token = tokens[pos];pos++;
        if( len == 6){
            if(tokencmp(token, "select")){
                return parseSelectStatement();
            }
            else if( tokencmp(token, "delete")){

            }
            else if( tokencmp(token, "insert")){
                return parseInsertStatement();
            }
            else if( tokencmp(token, "create")){

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


class TableFacade{
    BlockService * blockService;
public:
    TableFacade(BlockService * blockService)
    :blockService(blockService){}

    size_t findOne(void * key){
        //fuck everyone
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
        metaDataService = new MetaDataService(fileService);
        blockService = new BlockService(fileService);
        recordService = new RecordService(blockService);
    }

    void testSQL(){
        auto session = new SQLSession(metaDataService, nullptr, recordService, blockService);
        session->loadTable("test");
        auto parser = new SQLParser(session);
        parser->tokenize();
        parser->test();
        auto sql = parser->parseSQLStatement();
        std::cout<<sql->toJSON()->toString(true)<<endl;
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
            printf("scan %d => %s\n", id, ptr);
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
    applicationContainer.testSQL();
    int fid = applicationContainer.fileService->openFile("test.idx");
//    BPlusTree<char[16]> bPlusTree(applicationContainer.blockService, fid);
//    bPlusTree.T_BPLUS_TEST();
    //applicationContainer.testBlock();
   // applicationContainer.testQueryScanner(fid);
    return 0;
}



class QueryProjection {
protected:
    int st;
    std::string name;
public:
    QueryProjection(int st, const string &name) : st(st), name(name) {}

    virtual std::string project(void * data) = 0;
};

class IntegerQueryProjection: public QueryProjection{
public:

    IntegerQueryProjection(int st, const string &name) : QueryProjection(st, name) {}

    std::string project(void * data) override {
        char str[64];
        sprintf(str,"%d", *(int *)(((char*)data) + st));
        return std::string(str);
    }
};

class CharQueryProjection:public QueryProjection{
    int len;
public:
    CharQueryProjection(int st, const string &name, int len) : QueryProjection(st, name), len(len) {}

    string project(void * data) override {
        return std::string((char *)data, st,  len);
    }
};

class FloatQueryProjection: public QueryProjection{
public:
    FloatQueryProjection(int st, const string &name) : QueryProjection(st, name) {}

    string project(void * data) override {
        char str[64];
        sprintf(str,"%.3lf", *(double *)(((char*)data) + st));
        return std::string(str);
    }
};





/** 

    metablock datablock

**/ 