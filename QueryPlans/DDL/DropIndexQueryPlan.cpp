//
// Created by 张程易 on 04/06/2017.
//

#include "DropIndexQueryPlan.h"

DropIndexQueryPlan::DropIndexQueryPlan(const std::string &name, const std::string &table, SQLSession *sqlSession)
        : name(name), table(table), sqlSession(sqlSession) {}

JSON *DropIndexQueryPlan::toJSON() {
    auto json = new JSONObject();
    json->set("type", "drop");
    json->set("subType", "index");
    json->set("name", name);
    json->set("table", table);
    return json;
}

JSON *DropIndexQueryPlan::runQuery(RecordService *recordService) {
    auto db = sqlSession->getDataBaseModel();
    if(!db->hasTable(table)){
        return JSONMessage(-1, "Table `" + table + "` does not exists").toJSON();
    }
    auto tb = db->getTableByName(table);
    for(auto & ix: *(tb->getIndices())){
        if(ix.second.getName() == name){
            tb->dropIndex(name);
            sqlSession->saveMetaData();
            return JSONMessage(0, "acknowledged").toJSON();
        }
    }
    return JSONMessage(-1, "Index `" + name + "` does not exists").toJSON();

}
