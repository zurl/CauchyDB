//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_BASICSQLCONDITION_H
#define DB_BASICSQLCONDITION_H

#include "AbstractSQLCondition.h"
#include "SQLConditionUtils.h"

template<typename T>
class BasicSQLCondition : public AbstractSQLCondition{
protected:
    T value;
    int cid;
    int on;
    int size;
public:
    BasicSQLCondition(T value, int cid, int on, int size) : value(value), cid(cid), on(on), size(size) {}

    template<class Q = T>
    typename std::enable_if<std::is_same<Q, char *>::value>::type destroy()
    {
        delete value;
    }

    template<class Q = T>
    typename std::enable_if<!std::is_same<Q, char *>::value>::type destroy()
    {
    }

    ~BasicSQLCondition(){ destroy(); }

    virtual Type getType() override = 0;

    void * getValue() override {
        return SQLConditionUtils<T>::toVoid(value);
    }

    int getCid() override {
        return cid;
    }

    virtual bool filter(void *data) = 0;
    virtual JSON *toJSON(TableModel *tableModel){
        JSONObject * jobj = new JSONObject();
        jobj->set("value", SQLConditionUtils<T>::toJSON(value, size));
        return (JSON *)jobj;
    }
};


#endif //DB_BASICSQLCONDITION_H
