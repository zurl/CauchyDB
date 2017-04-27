//
// Created by 张程易 on 2017/3/21.
//

#ifndef DB_SQLPARSER_H
#define DB_SQLPARSER_H
#include "Common.h"
#include "SQLSession.h"
#include "QueryPlans/SQLCondition.h"
#include "QueryPlans/SelectQueryPlan.h"
#include "QueryPlans/InsertQueryPlan.h"
#include "QueryPlans/CreateQueryPlan.h"
#include "QueryPlans/CreateTableQueryPlan.h"
#include "QueryPlans/CreateDataBaseQueryPlan.h"
#include "Scanners/QueryScanner.h"
#include "Scanners/LinearQueryScanner.h"
#include "Scanners/OneIndexQueryScanner.h"
#include "Scanners/RangeIndexQueryScanner.h"

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
    void tokenize();


    bool tokencmp(Token & token, const char * target);

    std::list<SQLCondition> * parseWhereClause(TableModel * tableModel);

    std::pair<QueryScanner *, SQLWhereClause *>
    getQueryScanner(TableModel * table, std::list<SQLCondition> * list);


    SelectQueryPlan * parseSelectStatement();

    InsertQueryPlan * parseInsertStatement();

    void parseCreateDefinition(JSONArray * defJson, JSONObject * indexJson);

    CreateQueryPlan * parseCreateStatement();

    QueryPlan * parseSQLStatement(const char * str);

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
