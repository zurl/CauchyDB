//
// Created by 张程易 on 2017/3/21.
//

#ifndef DB_SQLPARSER_H
#define DB_SQLPARSER_H
#include "Common.h"
#include "SQLSession.h"
#include "QueryPlans/SelectQueryPlan.h"
#include "QueryPlans/InsertQueryPlan.h"
#include "QueryPlans/DDL/CreateQueryPlan.h"
#include "QueryPlans/DDL/CreateTableQueryPlan.h"
#include "QueryPlans/DDL/CreateDataBaseQueryPlan.h"
#include "Scanners/QueryScanner.h"
#include "Scanners/LinearQueryScanner.h"
#include "Scanners/OneIndexQueryScanner.h"
#include "Scanners/RangeIndexQueryScanner.h"
#include "QueryPlans/SQLCondition/SQLConditionFactory.h"
#include "QueryPlans/DDL/DropQueryPlan.h"

class SQLParser{
    SQLSession * sqlSession;
    const char * str = nullptr;// = "create table test2( id int, value char(20), primary key(id) );";
    //const char * str = "insert into test (id, value) values (20, '20');";
    //const char * str = "select id, value from test where value = '20' and value = 'F';";

    AbstractSQLConditionFactory * IntegerSQLConditionFactory = new SQLConditionFactory<int>();
    AbstractSQLConditionFactory * DoubleSQLConditionFactory = new SQLConditionFactory<double>();
    AbstractSQLConditionFactory * CharSQLConditionFactory = new SQLConditionFactory<char *>();

public:
    SQLParser(SQLSession *sqlSession) : sqlSession(sqlSession) {}
    ~SQLParser(){
        delete IntegerSQLConditionFactory;
        delete DoubleSQLConditionFactory;
        delete CharSQLConditionFactory;
    }

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
    int pos = 0;

    inline bool isNum(char x) {
        return x >= '0' && x <= '9';
    }
    inline bool isChar(char x) {
        return (x >= 'a' && x <= 'z') || (x >= 'A' && x <='Z') || x == '_' || x == '-';
    }
    void tokenize();


    bool tokencmp(Token & token, const char * target);

    std::list<AbstractSQLCondition *> * parseWhereClause(TableModel * tableModel);

    std::pair<QueryScanner *, SQLWhereClause *>
    getQueryScanner(TableModel * table, std::list<AbstractSQLCondition *> * list);


    SelectQueryPlan * parseSelectStatement();

    InsertQueryPlan * parseInsertStatement();

    void parseCreateDefinition(JSONArray * defJson, JSONObject * indexJson);

    CreateQueryPlan * parseCreateStatement();

    QueryPlan * parseSQLStatement(const char * str);

    DropQueryPlan *parseDropStatement() ;

    inline void test(){
        for(auto & x: tokens){
            for(int i = x.begin; i<= x.end; i++){
                putchar(str[i]);
            }
            printf("%d\n", x.end - x.begin + 1);
        }
    }

};


#endif //DB_SQLPARSER_H
