//
// Created by 张程易 on 2017/4/8.
//

#include "InsertQueryPlan.h"

InsertQueryPlan::InsertQueryPlan(TableModel *tableModel) : tableModel(tableModel), data(new char[tableModel->getLen()]){}
InsertQueryPlan::~InsertQueryPlan(){
    delete[] data;
}

void *InsertQueryPlan::getData() const {
    return data;
}

JSON * InsertQueryPlan::runQuery( RecordService * recordService)  {
    // insert records
    size_t recordId = recordService->insert(tableModel->getFid(), data, tableModel->getLen());
    auto iter = tableModel->getIndices()->begin();
    while( iter != tableModel->getIndices()->end()){
        size_t on = iter->second.getOn();
        iter->second.getIndexRunner()->insert((char *)data + on, recordId);
        iter ++;
    }
    return new JSONInteger(1);
}

JSON *InsertQueryPlan::toJSON()  {
    auto json = new JSONObject();
    json->hashMap.emplace("type", new JSONString("insert"));
    json->hashMap.emplace("table", new JSONString(tableModel->getName()));
    auto jarr = new JSONArray();
    char * cur = (char *)data;
    for(int i = 0; i < tableModel->getColumns().size(); i++){
        jarr->elements.emplace_back(ColumnTypeUtil::toJSON(
                tableModel->getColumn(i)->getType(),
                cur
        ));
        cur += tableModel->getColumn(i)->getSize();
    }
    json->hashMap.emplace("value", jarr);
    return json;
}
