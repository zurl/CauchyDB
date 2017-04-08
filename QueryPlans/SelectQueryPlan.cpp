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
    queryScanner->scan([this](size_t id, void *data){
        auto current = new JSONArray();
        for(auto & cid : columns){
            auto col = tableModel->getColumn(cid);
            current->elements.emplace_back(ColumnTypeUtil::toJSON(col->getType(), (char*)data+col->getOn()));
        }
    });
}

SelectQueryPlan::~SelectQueryPlan(){
    delete queryScanner;
    delete where;
}
JSON *SelectQueryPlan::toJSON()  {
    auto json = new JSONObject();
    auto col = new JSONArray();
    for(auto & x: columns){
        col->elements.emplace_back(new JSONInteger(x));
    }
    json->hashMap.emplace("type", new JSONString("select"));
    json->hashMap.emplace("table", new JSONString(tableModel->getName()));
    json->hashMap.emplace("scanner", queryScanner->toJSON());
    json->hashMap.emplace("columns", col);
    json->hashMap.emplace("where", where == nullptr ? new JSONNull() : where->toJSON(tableModel));
    return json;
}
