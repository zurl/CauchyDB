//
// Created by 张程易 on 04/06/2017.
//

#ifndef DB_DROPINDEXQUERYPLAN_H
#define DB_DROPINDEXQUERYPLAN_H


#include "DropQueryPlan.h"
#include "../../SQLSession.h"

class DropIndexQueryPlan: public DropQueryPlan {
    std::string name;
    std::string table;
    SQLSession * sqlSession;
public:
    DropIndexQueryPlan(const std::string &name, const std::string &table, SQLSession *sqlSession);

    JSON *toJSON() override;

    JSON *runQuery(RecordService *recordService) override;
};


#endif //DB_DROPINDEXQUERYPLAN_H
