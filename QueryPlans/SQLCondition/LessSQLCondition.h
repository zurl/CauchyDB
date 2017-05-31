// Created by 张程易 on 2017/4/26.
//

#ifndef DB_LessSQLCONDITION_H
#define DB_LessSQLCONDITION_H

#include "BasicSQLCondition.h"

template<typename T>
class LessSQLCondition : public BasicSQLCondition<T>{
public:
    LessSQLCondition(T value, int cid, int on, int size) : BasicSQLCondition<T>(value, cid, on, size) {}

    template<class Q = T>
    inline typename std::enable_if<!std::is_same<Q, char *>::value, bool>::type basicFilter(void * data)
    {
        return *(T *)((char *)data + BasicSQLCondition<T>::on) < BasicSQLCondition<T>::value;
    }

     template<class Q = T>
     inline typename std::enable_if<std::is_same<Q, char *>::value, bool>::type basicFilter(void * data)
    {
        char * str = ((char *) data) + BasicSQLCondition<T>::on;
        for(int i = 0; i < this->size; i++){
            if(str[i] > this->value[i]) return false;
            if(str[i] < this->value[i]) return true;
        }
        return false;
    }

    inline virtual bool filter(void * data) override {
        return this->basicFilter(data);
    }

    JSON *toJSON(TableModel *tableModel) override {
        JSONObject * jobj = (JSONObject * ) BasicSQLCondition<T>::toJSON(tableModel);
        jobj->set("type", "Less");
        return (JSON *) jobj;
    }

    AbstractSQLCondition::Type getType() override {
        return AbstractSQLCondition::Type::gt;
    }

    ~LessSQLCondition() override {
    }

};

#endif //DB_LessSQLCONDITION_H
