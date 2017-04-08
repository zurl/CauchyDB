//
// Created by 张程易 on 2017/4/8.
//

#include "CreateDataBaseQueryPlan.h"


CreateDataBaseQueryPlan::CreateDataBaseQueryPlan(const std::string &name, SQLSession *sqlSession) : name(name), sqlSession(sqlSession) {}

JSON *CreateDataBaseQueryPlan::runQuery(RecordService *recordService)  {
    sqlSession->getMetaDataService()->createDataBase(name);
    return new JSONInteger(0);
}

JSON *CreateDataBaseQueryPlan::toJSON()  {
    auto json = new JSONObject();
    json->hashMap.emplace("type", new JSONString("create"));
    json->hashMap.emplace("subType", new JSONString("database"));
    json->hashMap.emplace("name", new JSONString(name));
    return json;
}
