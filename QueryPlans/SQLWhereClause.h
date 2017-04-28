//
// Created by 张程易 on 2017/4/8.
//

#ifndef DB_SQLWHERECLAUSE_H
#define DB_SQLWHERECLAUSE_H

#include "SQLCondition/AbstractSQLCondition.h"


class SQLWhereClause{
    std::vector<AbstractSQLCondition *> conds;
public:
    void addCondition(AbstractSQLCondition * cond);
    JSON * toJSON(TableModel * tableModel);
    bool filter(void * data);
};


#endif //DB_SQLWHERECLAUSE_H
