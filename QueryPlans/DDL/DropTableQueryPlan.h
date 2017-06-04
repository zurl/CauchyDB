//
// Created by 张程易 on 03/06/2017.
//

#ifndef DB_DROPTABLEQUERYPLAN_H
#define DB_DROPTABLEQUERYPLAN_H


#include "../../JSON/JSON.h"
#include "../../SQLSession.h"
#include "DropQueryPlan.h"

class DropTableQueryPlan: public DropQueryPlan {
    std::string name;
    SQLSession * sqlSession;
public:
    DropTableQueryPlan(const std::string &name, SQLSession *sqlSession) ;

    JSON *toJSON() override ;

    JSON *runQuery(RecordService *recordService) override;
};


#endif //DB_DROPTABLEQUERYPLAN_H
