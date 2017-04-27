//
// Created by 张程易 on 2017/4/26.
//

#include "InterpreterQueryPlan.h"

JSON *InterpreterQueryPlan::toJSON() {
    JSONObject * jobj = new JSONObject();
    jobj->set("type", type);
    jobj->set("value", value);
}

JSON *InterpreterQueryPlan::runQuery(RecordService *recordService) {
    if(type == "using"){
        sqlSession->loadDatabase(value);
        return JSONMessage(0, "acknowledged").toJSON();
    }
    else if(type == "show"){
        if(value == "tables"){

        }
        else if(value == "database"){

        }
    }
    else if(type == "describe"){

    }
}

InterpreterQueryPlan::InterpreterQueryPlan(const std::string &type, const std::string &value, SQLSession *sqlSession)
        : type(type), value(value), sqlSession(sqlSession) {}
