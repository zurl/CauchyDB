//
// Created by 张程易 on 2017/4/8.
//

#ifndef DB_SELECTQUERYPLAN_H
#define DB_SELECTQUERYPLAN_H
#include "QueryPlan.h"
#include "SQLWhereClause.h"
#include "../Models/TableModel.h"
#include "../Scanners/QueryScanner.h"


class SelectQueryPlan : public QueryPlan{
    QueryScanner * queryScanner;
    TableModel * tableModel;
    std::vector<size_t> columns;
    SQLWhereClause * where;
public:
    SelectQueryPlan(TableModel * tableModel,
                    QueryScanner *queryScanner,
                    const std::vector<std::string> &columns,
                    SQLWhereClause *where);

    JSON *runQuery(RecordService *recordService) override;

    virtual ~SelectQueryPlan();
    JSON *toJSON() override ;
};
#endif //DB_SELECTQUERYPLAN_H
