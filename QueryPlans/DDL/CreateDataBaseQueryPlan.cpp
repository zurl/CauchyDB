//
// Created by 张程易 on 2017/4/8.
//

#include "CreateDataBaseQueryPlan.h"


CreateDataBaseQueryPlan::CreateDataBaseQueryPlan(const std::string &name, SQLSession *sqlSession) : name(name), sqlSession(sqlSession) {}

JSON *CreateDataBaseQueryPlan::runQuery(RecordService *recordService)  {
    if(sqlSession->hasDataBase(name)){
        return JSONMessage(-1, "Database `" + name + "` is already exists").toJSON();
    }
    sqlSession->getMetaDataService()->createDataBase(name);
    sqlSession->saveMetaData();
    return JSONMessage(0, "acknowledged").toJSON();
}

JSON *CreateDataBaseQueryPlan::toJSON()  {
    auto json = new JSONObject();
    json->set("type", "create");
    json->set("subType", "database");
    json->set("name", name);
    return json;
}
