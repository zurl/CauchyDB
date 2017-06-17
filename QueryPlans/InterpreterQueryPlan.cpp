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
    if(type == "use"){
        if(sqlSession->loadDatabase(value)){
            return JSONMessage(0, "acknowledged").toJSON();
        }else{
            return JSONMessage(-1, "No Such Database").toJSON();
        }
    }
    else if(type == "set"){
        if(value == "noflush"){
            sqlSession->setFlush(false);
            return JSONMessage(0, "acknowledged").toJSON();
        }
        else if(value == "flush"){
            sqlSession->setFlush(true);
            return JSONMessage(0, "acknowledged").toJSON();
        }
        else{
            return JSONMessage(-1, "unknown operation").toJSON();
        }
    }
    else if(type == "show"){
        JSONObject * result = new JSONObject();
        result->set("status", 2);
        JSON * jarr = nullptr;
        if(value == "tables"){
            if(sqlSession->getDataBaseModel() == nullptr)
                throw SQLExecuteException(5, "NO DATABASE Selected");
            jarr = sqlSession->getDataBaseModel()->getTables();
        }
        else if(value == "databases"){
            jarr = sqlSession->getMetaDataService()->getDatabases();
        }
        else if(value == "queryplan"){
            std::cout<<sqlSession->getLastQueryPlan()->toJSON()->toString(true, 2)<<std::endl;
            return JSONMessage(0, "acknowledged").toJSON();
        }
        else throw SQLExecuteException(5, "unknown operation");
        result->set("data", jarr);
        return result;
    }
    else throw SQLExecuteException(5, "unknown operation");
}

InterpreterQueryPlan::InterpreterQueryPlan(const std::string &type, const std::string &value, SQLSession *sqlSession)
        : type(type), value(value), sqlSession(sqlSession) {}
