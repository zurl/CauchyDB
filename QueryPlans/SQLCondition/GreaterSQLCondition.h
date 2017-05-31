//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_GREATERSQLCONDITION_H
#define DB_GREATERSQLCONDITION_H

#include "BasicSQLCondition.h"

template<typename T>
class GreaterSQLCondition : public BasicSQLCondition<T>{
public:
    GreaterSQLCondition(T value, int cid, int on, int size) : BasicSQLCondition<T>(value, cid, on, size) {}

    template<class Q = T>
    inline typename std::enable_if<!std::is_same<Q, char *>::value, bool>::type basicFilter(void * data)
    {
        return *(T *)((char *)data + BasicSQLCondition<T>::on) > BasicSQLCondition<T>::value;
    }

     template<class Q = T>
     inline typename std::enable_if<std::is_same<Q, char *>::value, bool>::type basicFilter(void * data)
    {
        char * str = ((char *) data) + BasicSQLCondition<T>::on;
        for(int i = 0; i < this->size; i++){
            if(str[i] > this->value[i]) return true;
            if(str[i] < this->value[i]) return false;
        }
        return false;
    }

    inline virtual bool filter(void * data) override {
        return this->basicFilter(data);
    }

    JSON *toJSON(TableModel *tableModel) override {
        JSONObject * jobj = (JSONObject * ) BasicSQLCondition<T>::toJSON(tableModel);
        jobj->set("type", "Greater");
        return (JSON *) jobj;
    }

    AbstractSQLCondition::Type getType() override {
        return AbstractSQLCondition::Type::gt;
    }

    ~GreaterSQLCondition() override {
    }

};

#endif //DB_GREATERSQLCONDITION_H
