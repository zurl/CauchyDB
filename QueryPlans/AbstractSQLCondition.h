//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_ABSTRACTSQLCONDITION_H
#define DB_ABSTRACTSQLCONDITION_H

#include "../JSON/JSON.h"
#include "../Models/TableModel.h"

class AbstractSQLCondition{
public:
    virtual bool filter(void * data) = 0;
    virtual ~AbstractSQLCondition() = 0;
    virtual JSON * toJSON(TableModel * tableModel);
};

#endif //DB_ABSTRACTSQLCONDITION_H
