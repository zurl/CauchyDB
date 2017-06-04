//
// Created by 张程易 on 03/06/2017.
//

#include "DropTableQueryPlan.h"




DropTableQueryPlan::DropTableQueryPlan(const std::string &name, SQLSession *sqlSession) : name(name), sqlSession(sqlSession) {}

JSON *DropTableQueryPlan::runQuery(RecordService *recordService)  {
    auto db = sqlSession->getDataBaseModel();
    if(!db->hasTable(name)){
        return JSONMessage(-1, "Table `" + name + "` does not exists").toJSON();
    }
    db->dropTable(name);
    sqlSession->saveMetaData();
    return JSONMessage(0, "acknowledged").toJSON();
}

JSON *DropTableQueryPlan::toJSON()  {
    auto json = new JSONObject();
    json->set("type", "drop");
    json->set("subType", "table");
    json->set("name", name);
    return json;
}