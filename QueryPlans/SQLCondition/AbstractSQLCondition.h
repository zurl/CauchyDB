//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_ABSTRACTSQLCONDITION_H
#define DB_ABSTRACTSQLCONDITION_H

#include "../../JSON/JSON.h"
#include "../../Models/TableModel.h"

class AbstractSQLCondition{
public:
    enum class Type{
        gt, lt, gte, lte, eq, neq
    };
    virtual bool filter(void * data) = 0;
    virtual ~AbstractSQLCondition(){}
    virtual JSON * toJSON(TableModel * tableModel) = 0;
    virtual void * getValue() = 0;
    virtual Type getType() = 0;
    virtual int getCid() = 0;
};

#endif //DB_ABSTRACTSQLCONDITION_H
