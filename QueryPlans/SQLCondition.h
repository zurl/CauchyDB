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
    int on;
    ColumnType columnType;
    int columnSize;

    SQLCondition(TableModel * tableModel, Type type, int cid, void *value);
    JSON * toJSON(TableModel * tableModel);
    bool filter(void * data);
};


#endif //DB_SQLCONDITION_H
