//
// Created by 张程易 on 11/06/2017.
//

#ifndef DB_DELETEQUERYPLAN_H
#define DB_DELETEQUERYPLAN_H


#include "../Scanners/QueryScanner.h"
#include "SQLWhereClause.h"
#include "QueryPlan.h"

class DeleteQueryPlan: public QueryPlan {
    QueryScanner * queryScanner;
    TableModel * tableModel;
    SQLWhereClause * where;
public:
    DeleteQueryPlan(QueryScanner *queryScanner, TableModel *tableModel, SQLWhereClause *where);

    JSON *runQuery(RecordService *recordService) override;

    virtual ~DeleteQueryPlan();
    JSON *toJSON() override ;
};


#endif //DB_DELETEQUERYPLAN_H
