//
// Created by 张程易 on 03/06/2017.
//

#include "DropDataBaseQueryPlan.h"



DropDataBaseQueryPlan::DropDataBaseQueryPlan(const std::string &name, SQLSession *sqlSession) : name(name), sqlSession(sqlSession) {}

JSON *DropDataBaseQueryPlan::runQuery(RecordService *recordService)  {
    if(!sqlSession->hasDataBase(name)){
        return JSONMessage(-1, "Database `" + name + "` does not exists").toJSON();
    }
    sqlSession->getMetaDataService()->dropDataBase(name);
    sqlSession->saveMetaData();
    return JSONMessage(0, "acknowledged").toJSON();

}

JSON *DropDataBaseQueryPlan::toJSON()  {
    auto json = new JSONObject();
    json->set("type", "drop");
    json->set("subType", "database");
    json->set("name", name);
    return json;
}
