//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_BASICSQLCONDITION_H
#define DB_BASICSQLCONDITION_H

#include "AbstractSQLCondition.h"

template<typename T>
class BasicSQLCondition : public AbstractSQLCondition{
protected:
    T value;
    int on;
    int size;
public:
    BasicSQLCondition(T value, int on, int size) : value(value), on(on), size(size) {}
    virtual ~BasicSQLCondition(){ delete value; }
    virtual bool filter(void *data) = 0;
};


#endif //DB_BASICSQLCONDITION_H
