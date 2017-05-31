//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_EqualSQLCONDITION_H
#define DB_EqualSQLCONDITION_H

#include "BasicSQLCondition.h"

template<typename T>
class EqualSQLCondition : public BasicSQLCondition<T>{
public:
    EqualSQLCondition(T value, int cid, int on, int size) : BasicSQLCondition<T>(value, cid, on, size) {}

    template<class Q = T>
    inline typename std::enable_if<!std::is_same<Q, char *>::value, bool>::type basicFilter(void * data)
    {
        return *(T *)((char *)data + BasicSQLCondition<T>::on) == BasicSQLCondition<T>::value;
    }

     template<class Q = T>
     inline typename std::enable_if<std::is_same<Q, char *>::value, bool>::type basicFilter(void * data)
    {
        char * str = ((char *) data) + BasicSQLCondition<T>::on;
        for(int i = 0; i < this->size; i++){
            if(str[i] > this->value[i]) return false;
            if(str[i] < this->value[i]) return false;
        }
        return true;
    }

    inline virtual bool filter(void * data) override {
        return this->basicFilter(data);
    }

    JSON *toJSON(TableModel *tableModel) override {
        JSONObject * jobj = (JSONObject * ) BasicSQLCondition<T>::toJSON(tableModel);
        jobj->set("type", "Equal");
        return (JSON *) jobj;
    }

    AbstractSQLCondition::Type getType() override {
        return AbstractSQLCondition::Type::gt;
    }

    ~EqualSQLCondition() override {
    }

};

#endif //DB_EqualSQLCONDITION_H
