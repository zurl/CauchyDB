//
// Created by 张程易 on 04/06/2017.
//

#include "CreateIndexQueryPlan.h"

CreateIndexQueryPlan::CreateIndexQueryPlan(const std::string &column, const std::string &name, const std::string &table,
                                           SQLSession *sqlSession) : column(column), name(name), table(table),
                                                                     sqlSession(sqlSession) {}

JSON *CreateIndexQueryPlan::toJSON() {
    auto json = new JSONObject();
    json->set("type", "create");
    json->set("subType", "index");
    json->set("name", name);
    json->set("table", table);
    json->set("column", column);
    return json;
}

JSON *CreateIndexQueryPlan::runQuery(RecordService *recordService) {
    auto db = sqlSession->getDataBaseModel();
    if(db->hasTable(table)){
        return JSONMessage(-1, "Table `" + table + "` already exists").toJSON();
    }
    auto tb = db->getTableByName(table);
    int id = tb->getColumnIndex(column);
    auto ix = tb->getIndices();
    auto iter = (*ix).find(id);
    if(iter != (*ix).end()){
        return JSONMessage(-1, "Column index `" + table + "` already exists").toJSON();
    }
    for(auto & idx: *ix){
        if(idx.second.getName() == name){
            return JSONMessage(-1, "Index `" + name + "` already exists").toJSON();
        }
    }
    tb->createIndex(name, column);
    return JSONMessage(0, "acknowledged").toJSON();
}
