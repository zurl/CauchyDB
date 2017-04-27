//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_INTERPRETERQUERYPLAN_H
#define DB_INTERPRETERQUERYPLAN_H


#include "QueryPlan.h"
#include "../SQLSession.h"

class InterpreterQueryPlan : public QueryPlan{
    std::string type;
    std::string value;
    SQLSession * sqlSession;
public:
    InterpreterQueryPlan(const std::string &type, const std::string &value, SQLSession *sqlSession);

    JSON *toJSON() override;

    JSON *runQuery(RecordService *recordService) override;
};


#endif //DB_INTERPRETERQUERYPLAN_H
