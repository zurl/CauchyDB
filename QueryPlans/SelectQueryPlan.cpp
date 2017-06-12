//
// Created by 张程易 on 2017/4/8.
//

#include "SelectQueryPlan.h"


SelectQueryPlan::SelectQueryPlan(TableModel * tableModel,
                QueryScanner *queryScanner,
                const std::vector<std::string> &columns,
                SQLWhereClause *where)
        :queryScanner(queryScanner), tableModel(tableModel),  where(where) {
    if(columns.size() == 0){
        //select * from table
        for(int i = 0; i < tableModel->getColumnCnt(); i ++){
            this->columns.emplace_back(i);
        }
    }
    else{
        for(auto & key : columns){
            this->columns.emplace_back(tableModel->getColumnIndex(key));
        }
    }
}

JSON *SelectQueryPlan::runQuery(RecordService *recordService)  {
    auto json = new JSONObject();
    auto result = new JSONArray();
    auto title = new JSONArray();
    for(auto & column: columns){
        title->put(new JSONString(tableModel->getColumn(column)->getName()));
    }
    queryScanner->scan([this, result](int id, void *data){
        if(where != nullptr && !where->filter(data))return false;
        auto current = new JSONArray();
        for(auto & cid : columns){
            auto col = tableModel->getColumn(cid);
            current->put(ColumnTypeUtil::toJSON(col->getType(), (char*)data+col->getOn()));
        }
        result->put(current);
        return false;
    });
    json->set("status", 2);
    json->set("data", result);
    return json;
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
