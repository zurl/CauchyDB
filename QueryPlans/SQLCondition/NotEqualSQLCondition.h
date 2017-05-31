//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_NotEqualSQLCONDITION_H
#define DB_NotEqualSQLCONDITION_H

#include "BasicSQLCondition.h"

template<typename T>
class NotEqualSQLCondition : public BasicSQLCondition<T>{
public:
    NotEqualSQLCondition(T value, int cid, int on, int size) : BasicSQLCondition<T>(value, cid, on, size) {}

    template<class Q = T>
    inline typename std::enable_if<!std::is_same<Q, char *>::value, bool>::type basicFilter(void * data)
    {
        return *(T *)((char *)data + BasicSQLCondition<T>::on) != BasicSQLCondition<T>::value;
    }

     template<class Q = T>
     inline typename std::enable_if<std::is_same<Q, char *>::value, bool>::type basicFilter(void * data)
    {
        char * str = ((char *) data) + BasicSQLCondition<T>::on;
        for(int i = 0; i < this->size; i++){
            if(str[i] > this->value[i]) return true;
            if(str[i] < this->value[i]) return true;
        }
        return false;
    }

    inline virtual bool filter(void * data) override {
        return this->basicFilter(data);
    }

    JSON *toJSON(TableModel *tableModel) override {
        JSONObject * jobj = (JSONObject * ) BasicSQLCondition<T>::toJSON(tableModel);
        jobj->set("type", "NotEqual");
        return (JSON *) jobj;
    }

    AbstractSQLCondition::Type getType() override {
        return AbstractSQLCondition::Type::gt;
    }

    ~NotEqualSQLCondition() override {
    }

};

#endif //DB_NotEqualSQLCONDITION_H
