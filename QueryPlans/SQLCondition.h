//
// Created by 张程易 on 2017/4/8.
//

#ifndef DB_SQLCONDITION_H
#define DB_SQLCONDITION_H
#include "../Common.h"
#include "../Models/TableModel.h"

class SQLCondition{
public:
    enum class Type{
        gt, lt, gte, lte, eq, neq
    };
    const char * TypeName[6] = {
            "gt", "lt", "gte", "lte", "eq", "neq"
    };
    Type type;
    int cid;
    void * value;
    SQLCondition(Type type, int cid, void *value);
    JSON * toJSON(TableModel * tableModel);
};


#endif //DB_SQLCONDITION_H
