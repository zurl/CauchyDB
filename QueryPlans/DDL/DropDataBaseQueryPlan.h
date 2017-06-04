//
// Created by 张程易 on 03/06/2017.
//

#ifndef DB_DROPDATABASEQUERYPLAN_H
#define DB_DROPDATABASEQUERYPLAN_H


#include <string>
#include "../../SQLSession.h"
#include "../../JSON/JSON.h"
#include "DropQueryPlan.h"

class DropDataBaseQueryPlan : public DropQueryPlan{
    std::string name;
    SQLSession * sqlSession;
public:
    DropDataBaseQueryPlan(const std::string &name, SQLSession *sqlSession) ;

    JSON *runQuery(RecordService *recordService) override ;

    JSON *toJSON() override;
};


#endif //DB_DROPDATABASEQUERYPLAN_H
