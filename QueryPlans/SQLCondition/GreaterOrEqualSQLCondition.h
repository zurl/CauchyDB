//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_GreaterOrEqualSQLCONDITION_H
#define DB_GreaterOrEqualSQLCONDITION_H

#include "BasicSQLCondition.h"

template<typename T>
class GreaterOrEqualSQLCondition : public BasicSQLCondition<T>{
public:
    GreaterOrEqualSQLCondition(T value, int cid, int on, int size) : BasicSQLCondition<T>(value, cid, on, size) {}

    bool filter(void *data) override {
        return *(T *)data >= BasicSQLCondition<T>::value;
    }
    JSON *toJSON(TableModel *tableModel) override {
        JSONObject * jobj = (JSONObject * ) BasicSQLCondition<T>::toJSON(tableModel);
        jobj->set("type", "GreaterOrEqual");
        return (JSON *) jobj;
    }

    AbstractSQLCondition::Type getType() override {
        return AbstractSQLCondition::Type::gte;
    }
};

template<>
class GreaterOrEqualSQLCondition<char *> : public BasicSQLCondition<char *>{
public:
    GreaterOrEqualSQLCondition(char *value, int cid, int on, int size) : BasicSQLCondition(value, cid, on, size) {}
    bool filter(void *data) override {
        char * str = (char *) data;
        for(int i = 0; i < size; i++){
            if(str[i] > value[i]) return true;
            if(str[i] < value[i]) return false;
        }
        return true;
    }
};


#endif //DB_GreaterOrEqualSQLCONDITION_H
