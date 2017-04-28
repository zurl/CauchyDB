//
// Created by 张程易 on 2017/4/8.
//

#include "InsertQueryPlan.h"

InsertQueryPlan::InsertQueryPlan(TableModel *tableModel) : tableModel(tableModel), data(new char[tableModel->getLen()]){}
InsertQueryPlan::~InsertQueryPlan(){
    delete[] (char *)data;
}

void *InsertQueryPlan::getData() const {
    return data;
}

JSON * InsertQueryPlan::runQuery( RecordService * recordService)  {
    // insert records
    int recordId = recordService->insert(tableModel->getFid(), data, tableModel->getLen());
    auto iter = tableModel->getIndices()->begin();
    while( iter != tableModel->getIndices()->end()){
        int on = iter->second.getOn();
        iter->second.getIndexRunner()->insert((char *)data + on, recordId);
        iter ++;
    }
    return new JSONInteger(1);
}

JSON *InsertQueryPlan::toJSON()  {
    auto json = new JSONObject();
    json->set("type", "insert");
    json->set("table", tableModel->getName());
    auto jarr = new JSONArray();
    char * cur = (char *)data;
    for(int i = 0; i < tableModel->getColumns().size(); i++){
        jarr->put(ColumnTypeUtil::toJSON(
                tableModel->getColumn(i)->getType(),
                cur
        ));
        cur += tableModel->getColumn(i)->getSize();
    }
    json->set("value", jarr);
    return json;
}
