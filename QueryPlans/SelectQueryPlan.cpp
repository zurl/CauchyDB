//
// Created by 张程易 on 2017/4/8.
//

#include "SelectQueryPlan.h"


SelectQueryPlan::SelectQueryPlan(TableModel * tableModel,
                QueryScanner *queryScanner,
                const std::vector<std::string> &columns,
                SQLWhereClause *where)
        : tableModel(tableModel), queryScanner(queryScanner), where(where) {
    for(auto & key : columns){
        this->columns.emplace_back(tableModel->getColumnIndex(key));
    }
}

JSON *SelectQueryPlan::runQuery(RecordService *recordService)  {
    auto result = new JSONArray();
    queryScanner->scan([this, result](size_t id, void *data){
        auto current = new JSONArray();
        for(auto & cid : columns){
            auto col = tableModel->getColumn(cid);
            current->put(ColumnTypeUtil::toJSON(col->getType(), (char*)data+col->getOn()));
        }
        result->put(current);
    });
    return result;
}

SelectQueryPlan::~SelectQueryPlan(){
    delete queryScanner;
    delete where;
}
JSON *SelectQueryPlan::toJSON()  {
    auto json = new JSONObject();
    auto col = new JSONArray();
    for(auto & x: columns){
        col->put(new JSONInteger(x));
    }
    json->set("type", "select");
    json->set("table", tableModel->getName());
    json->set("scanner", queryScanner->toJSON());
    json->set("columns", col);
    json->set("where", where == nullptr ? (JSON *)new JSONNull() : where->toJSON(tableModel));
    return json;
}
