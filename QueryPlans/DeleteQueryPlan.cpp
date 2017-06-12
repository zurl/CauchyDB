//
// Created by 张程易 on 11/06/2017.
//

#include "DeleteQueryPlan.h"


JSON *DeleteQueryPlan::runQuery(RecordService *recordService) {
    auto result = new JSONArray();
    std::vector<void *> target;
    queryScanner->scan([this, result, &target](int id, void *data){
        if(where != nullptr && !where->filter(data)) return false;
        void * dst = malloc(tableModel->getLen());
        memcpy(dst, data, tableModel->getLen());
        target.emplace_back(dst);
        return true;
    });
    auto indices = tableModel->getIndices();
    for(auto & idpair: *indices){
        auto & index = idpair.second;
        AbstractIndexRunner * ir = index.getIndexRunner();
        for(auto & data: target){
            ir->remove((char *)data + index.getOn());
        }
    }
    for(auto & data: target){
        free(data);
    }
    return JSONIntegerMessage((int)target.size()).toJSON();
}

DeleteQueryPlan::~DeleteQueryPlan() {
    delete queryScanner;
    delete where;
}

JSON *DeleteQueryPlan::toJSON() {
    auto json = new JSONObject();
    json->set("type", "select");
    json->set("table", tableModel->getName());
    json->set("scanner", queryScanner->toJSON());
    json->set("where", where == nullptr ? (JSON *)new JSONNull() : where->toJSON(tableModel));
    return json;
}

DeleteQueryPlan::DeleteQueryPlan(QueryScanner *queryScanner, TableModel *tableModel, SQLWhereClause *where)
        : queryScanner(queryScanner), tableModel(tableModel), where(where) {}
