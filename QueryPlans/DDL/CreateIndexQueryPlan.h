//
// Created by 张程易 on 04/06/2017.
//

#ifndef DB_CREATEINDEXQUERYPLAN_H
#define DB_CREATEINDEXQUERYPLAN_H


#include "CreateQueryPlan.h"
#include "../../SQLSession.h"

class CreateIndexQueryPlan: public CreateQueryPlan {
    std::string column;
    std::string name;
    std::string table;
    SQLSession * sqlSession;
public:
    CreateIndexQueryPlan(const std::string &column, const std::string &name, const std::string &table,
                         SQLSession *sqlSession);

    JSON *toJSON() override;

    JSON *runQuery(RecordService *recordService) override;
};


#endif //DB_CREATEINDEXQUERYPLAN_H
