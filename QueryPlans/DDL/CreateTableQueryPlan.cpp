//
// Created by 张程易 on 2017/4/8.
//

#include "CreateTableQueryPlan.h"

CreateTableQueryPlan::CreateTableQueryPlan(const std::string &name, SQLSession *sqlSession, JSONArray *def, JSONObject *indices) : name(name),
                                                                                                        sqlSession(sqlSession)
{
    config = new JSONObject();
    config->set("columns", def);
    config->set("indices", indices);
}

JSON *CreateTableQueryPlan::toJSON()  {
    auto json = new JSONObject();
    json->set("type", "create");
    json->set("subType", "table");
    json->set("name", name);
    json->set("config", config);
    return json;
}

JSON * CreateTableQueryPlan::runQuery(RecordService *recordService)  {
    auto db = sqlSession->getDataBaseModel();
    if(db->hasTable(name)){
        return JSONMessage(-1, "Table `" + name + "` is already exists").toJSON();
    }
    db->createTable(name, config);
    sqlSession->saveMetaData();
    return JSONMessage(0, "acknowledged").toJSON();
}

CreateTableQueryPlan::~CreateTableQueryPlan() {
    delete config;
}

