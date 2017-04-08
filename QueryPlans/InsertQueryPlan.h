//
// Created by 张程易 on 2017/4/8.
//

#ifndef DB_INSERTQUERYPLAN_H
#define DB_INSERTQUERYPLAN_H
#include "QueryPlan.h"
#include "../Models/TableModel.h"

class InsertQueryPlan : public QueryPlan{
    TableModel * tableModel;
    void * data;
public:
    InsertQueryPlan(TableModel *tableModel);
    ~InsertQueryPlan() ;
    void *getData() const ;
    JSON * runQuery( RecordService * recordService) override ;
    JSON *toJSON() override ;
};



#endif //DB_INSERTQUERYPLAN_H
