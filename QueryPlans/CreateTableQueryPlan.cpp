//
// Created by 张程易 on 2017/4/8.
//

#include "CreateTableQueryPlan.h"

CreateTableQueryPlan::CreateTableQueryPlan(const std::string &name, SQLSession *sqlSession, JSONArray *def, JSONObject *indices) : name(name),
                                                                                                        sqlSession(
                                                                                                                sqlSession)
{
    config = new JSONObject();
    config->hashMap.emplace("columns", def);
    config->hashMap.emplace("indices", indices);
}

JSON *CreateTableQueryPlan::toJSON()  {
    auto json = new JSONObject();
    json->hashMap.emplace("type", new JSONString("create"));
    json->hashMap.emplace("subType", new JSONString("table"));
    json->hashMap.emplace("name", new JSONString(name));
    json->hashMap.emplace("config", config);
    return json;
}

JSON *CreateTableQueryPlan::runQuery(RecordService *recordService)  {
    auto db = sqlSession->getDataBaseModel();
    db->createTable(name, config);
    return new JSONInteger(0);
}

