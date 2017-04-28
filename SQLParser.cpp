//
// Created by 张程易 on 2017/3/21.
//

#include "SQLParser.h"
#include "QueryPlans/SQLCondition/SQLConditionFactory.h"
#include "QueryPlans/InterpreterQueryPlan.h"

InsertQueryPlan *SQLParser::parseInsertStatement() {
    //auto insertQueryPlan = new InsertQueryPlan();
    Token token = tokens[pos];
    std::vector<int> columns;
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
            int len = columnModel->getSize();
            int real_len = std::min(token.end - token.begin + 1, (int)(len - 1));
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

void SQLParser::parseCreateDefinition(JSONArray *defJson, JSONObject *indexJson) {
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

CreateQueryPlan *SQLParser::parseCreateStatement() {
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

QueryPlan *SQLParser::parseSQLStatement(const char *str) {
    this->str = str;
    tokens.clear();
    this->tokenize();
    pos = 0;
    Token token = tokens[pos];pos++;
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
    else if( tokencmp(token, "using")){
        token = tokens[pos]; pos ++;// database;
        if(token.type != TokenType::name)throw SQLSyntaxException(2, "syntax error");
        std::string name(str, token.begin, token.end - token.begin + 1);
        return new InterpreterQueryPlan("using", name, sqlSession);
    }
    else if( tokencmp(token, "show")){
        token = tokens[pos]; pos ++;// database;
        if(token.type != TokenType::name)throw SQLSyntaxException(2, "syntax error");
        std::string name(str, token.begin, token.end - token.begin + 1);
        return new InterpreterQueryPlan("show", name, sqlSession);
    }
    else if( tokencmp(token, "describe")){
        token = tokens[pos]; pos ++;// database;
        if(token.type != TokenType::name)throw SQLSyntaxException(2, "syntax error");
        std::string name(str, token.begin, token.end - token.begin + 1);
        return new InterpreterQueryPlan("describe", name, sqlSession);
    }
    else{
        throw SQLSyntaxException(0, "keyword error");
    }
    return nullptr;
}

void SQLParser::tokenize() {
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

bool SQLParser::tokencmp(SQLParser::Token &token, const char *target) {
    int len1 = token.end - token.begin + 1;
    int len2 = (int)strlen(target);
    if( len1 != len2 ) return false;
    return strncasecmp(str + token.begin, target, (int)len1) == 0;
}

std::list<AbstractSQLCondition *> *SQLParser::parseWhereClause(TableModel *tableModel) {
    Token token = tokens[pos];
    auto conditionList = new std::list<AbstractSQLCondition *>;
    try {
        while (token.type == TokenType::name) {
            int cid = tableModel->getColumnIndex(std::string(str, token.begin, token.end - token.begin + 1));
            AbstractSQLCondition::Type type;
            pos ++; token = tokens[pos]; // col
            Token ope = token;
            pos ++; token = tokens[pos]; // op
            void * data;
            ColumnModel * columnModel = tableModel->getColumn(cid);
            AbstractSQLConditionFactory * sqlConditionFactory;
            if(token.type == TokenType::integer){
                if( columnModel->getType() != ColumnType::Int ) throw SQLTypeException(3, "type error");
                data = new int[1];
                sscanf(str + token.begin, "%d", (int *)data);
                sqlConditionFactory = IntegerSQLConditionFactory;
            }
            else if(token.type == TokenType::string){
                if( columnModel->getType() != ColumnType::Char ) throw SQLTypeException(3, "type error");
                int len = columnModel->getSize();
                data = new char[len + 1];
                memcpy(data, str + token.begin, token.end - token.begin + 1);
                ((char *)data)[token.end - token.begin + 1] = 0;
                sqlConditionFactory = CharSQLConditionFactory;
            }
            else if(token.type == TokenType::real){
                if( columnModel->getType() != ColumnType::Float ) throw SQLTypeException(3, "type error");
                data = new double[1];
                sscanf(str + token.begin, "%lf", (double *)data);
                sqlConditionFactory = DoubleSQLConditionFactory;
            }
            else throw new SQLSyntaxException(0, "illegal operand");

            if(tokencmp(ope, "<")){
                type = AbstractSQLCondition::Type::lt;
            }
            else if(tokencmp(ope, ">")){
                type = AbstractSQLCondition::Type::gt;
            }
            else if(tokencmp(ope, ">=")){
                type = AbstractSQLCondition::Type::gte;
            }
            else if(tokencmp(ope, "<=")){
                type = AbstractSQLCondition::Type::lte;
            }
            else if(tokencmp(ope, "<>")){
                type = AbstractSQLCondition::Type::neq;
            }
            else if(tokencmp(ope, "=")){
                type = AbstractSQLCondition::Type::eq;
            }
            else throw new SQLSyntaxException(0, "illegal opes");

            conditionList->emplace_back(sqlConditionFactory->createSQLCondition(
                  type,
                  columnModel->getSize(),
                  cid,
                  columnModel->getOn(),
                  data
            ));
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

std::pair<QueryScanner *, SQLWhereClause *> SQLParser::getQueryScanner(TableModel *table, std::list<AbstractSQLCondition *> *list) {
    int status = 0;
    AbstractSQLCondition *l = nullptr, *r = nullptr;
    IndexModel *sidx;
    // 0 => no 1 => findone  2=> left 3=>right 4=>findtwo
    SQLWhereClause * where = new SQLWhereClause();
    QueryScanner * scanner = nullptr;
    for(auto & item : *list){
        if(status == 1 || status == 4){
            where->addCondition(item);
            continue;
        }
        IndexModel * idx = table->findIndexOn(item->getCid());
        if(status == 0){
            if(idx != nullptr){
                if(item->getType() == AbstractSQLCondition::Type::gt
                   ||item->getType() == AbstractSQLCondition::Type::gte){
                    l = item;
                    status = 2;
                    sidx = idx;
                    continue;
                }
                if(item->getType() == AbstractSQLCondition::Type::lt
                   ||item->getType() == AbstractSQLCondition::Type::lte){
                    r = item;
                    status = 3;
                    sidx = idx;
                    continue;
                }
                if(item->getType() == AbstractSQLCondition::Type::eq){
                    l = item;
                    status = 1;
                    sidx = idx;
                    continue;
                }
            }
        }
        else if(status == 2
                && idx == sidx
                && (item->getType() == AbstractSQLCondition::Type::lt
                    ||item->getType() == AbstractSQLCondition::Type::lte)){
            r = item;
            status = 4;
            continue;
        }
        else  if(status == 3
                 && idx == sidx
                 && (item->getType() == AbstractSQLCondition::Type::gt
                     ||item->getType() == AbstractSQLCondition::Type::gte)) {
            l = item;
            status = 4;
            continue;
        }
        where->addCondition(item);

    }
    if(status == 1){
        scanner = new OneIndexQueryScanner(
                sidx->getIndexRunner(),
                sqlSession->getRecordService(),
                table->getLen(),
                table->getFid(),
                l->getValue(),
                table->getColumn(l->getCid())->getName()
        );
    }
    else if(status == 2 ||status == 3 || status == 4){
        scanner = new RangeIndexQueryScanner(
                sidx->getIndexRunner(),
                sqlSession->getRecordService(),
                table->getLen(),
                table->getFid(),
                l == nullptr ? l : l->getValue(),
                r == nullptr ? r : r->getValue(),
                l == nullptr ? true : l->getType() == AbstractSQLCondition::Type::gte,
                r == nullptr ? true : r->getType() == AbstractSQLCondition::Type::lte,
                l != nullptr,
                r != nullptr,
                table->getColumn(l->getCid())->getName()
        );
    }
    // === select index ===
    delete list;
    return std::make_pair(scanner, where);
}

SelectQueryPlan * SQLParser::parseSelectStatement() {
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