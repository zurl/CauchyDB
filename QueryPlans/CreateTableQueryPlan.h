//
// Created by 张程易 on 2017/4/8.
//

#ifndef DB_CREATETABLEQUERYPLAN_H
#define DB_CREATETABLEQUERYPLAN_H

#include "QueryPlan.h"
#include "CreateQueryPlan.h"
#include "../SQLSession.h"


class CreateTableQueryPlan : public CreateQueryPlan{
    std::string name;
    SQLSession * sqlSession;
    JSONObject * config;
public:
    CreateTableQueryPlan(const std::string &name, SQLSession *sqlSession, JSONArray *def, JSONObject *indices) ;


    JSON *toJSON() override ;

    JSON *runQuery(RecordService *recordService) override;
};


#endif //DB_CREATETABLEQUERYPLAN_H
