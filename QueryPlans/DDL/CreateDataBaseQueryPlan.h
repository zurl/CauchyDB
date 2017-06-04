//
// Created by 张程易 on 2017/4/8.
//

#ifndef DB_CREATEDATABASEQUERYPLAN_H
#define DB_CREATEDATABASEQUERYPLAN_H
#include "CreateQueryPlan.h"
#include "../QueryPlan.h"
#include "../../SQLSession.h"

class CreateDataBaseQueryPlan: public CreateQueryPlan{
    std::string name;
    SQLSession * sqlSession;
public:
    CreateDataBaseQueryPlan(const std::string &name, SQLSession *sqlSession) ;

    JSON *runQuery(RecordService *recordService) override ;

    JSON *toJSON() override;
};



#endif //DB_CREATEDATABASEQUERYPLAN_H
