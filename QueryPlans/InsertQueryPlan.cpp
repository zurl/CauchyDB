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
    // check unique
    auto & columns = tableModel->getColumns();
    for(int i = 0; i < columns.size(); i++){
        if(columns[i].isUnique()){
            auto iter = tableModel->getIndices()->find(i);
            if( iter == tableModel->getIndices()->end()) {
                // no index
                bool flag = false;
                ColumnType columnType = columns[i].getType();
                int columnSize = columns[i].getSize();
                int on = columns[i].getOn();
                void * theData = this->data;
                if( columnType == ColumnType::Int){
                    recordService->scan(tableModel->getFid(), tableModel->getLen(),[&flag, on, theData](int offset, void * row){
                        if(flag) return false;
                        if(TypeUtil<int>::cmp(*(int*)((char *)theData + on), *(int*)((char *)row + on)) == 0){
                            flag = true;
                        }
                        return false;
                    });
                }
                else if(columnType == ColumnType::Float){
                    recordService->scan(tableModel->getFid(), tableModel->getLen(),[&flag, on, theData](int offset, void * row){
                        if(flag) return false;
                        if(TypeUtil<double>::cmp(*(double*)((char *)theData + on), *(double*)((char *)row + on)) == 0){
                            flag = true;
                        }
                        return false;
                    });
                }
                else{
                    recordService->scan(tableModel->getFid(), tableModel->getLen(),[&flag, on, theData, columnSize](int offset, void * row){
                        if(flag) return false;
                        if(strncmp((char *)theData + on, (char *)row + on, columnSize)){
                            flag = true;
                        }
                        return false;
                    });
                }
                if(flag){
                    return JSONMessage(-1, "The " + columns[i].getName() + " field is not unique").toJSON();
                }
            }
            else {
                // with index
                auto & index = iter->second;
                int result = index.getIndexRunner()->findOne((char *)data + columns[i].getOn());
                if(result != 0){
                    return JSONMessage(-1, "The " + columns[i].getName() + " field is not unique").toJSON();
                }
            }
        }
    }
    // insert records
    int recordId = recordService->insert(tableModel->getFid(), data, tableModel->getLen());
    auto iter = tableModel->getIndices()->begin();
    while( iter != tableModel->getIndices()->end()){
        int on = iter->second.getOn();
        iter->second.getIndexRunner()->insert((char *)data + on, recordId);
        iter ++;
    }
    return JSONMessage(0, "acknowledged").toJSON();
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
