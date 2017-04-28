//
// Created by 张程易 on 2017/4/26.
//

#include "InterpreterQueryPlan.h"

JSON *InterpreterQueryPlan::toJSON() {
    JSONObject * jobj = new JSONObject();
    jobj->set("type", type);
    jobj->set("value", value);
    return jobj;
}

JSON *InterpreterQueryPlan::runQuery(RecordService *recordService) {
    if(type == "using"){
        if(sqlSession->loadDatabase(value)){
            return JSONMessage(0, "acknowledged").toJSON();
        }else{
            return JSONMessage(-1, "No Such Database").toJSON();
        }
    }
    else if(type == "show"){
        if(value == "tables"){
            return sqlSession->getDataBaseModel()->getTables();
        }
        else if(value == "databases"){
            return sqlSession->getMetaDataService()->getDatabases();
        }
    }
    else if(type == "describe"){
        return nullptr;
    }
    return nullptr;
}

InterpreterQueryPlan::InterpreterQueryPlan(const std::string &type, const std::string &value, SQLSession *sqlSession)
        : type(type), value(value), sqlSession(sqlSession) {}
