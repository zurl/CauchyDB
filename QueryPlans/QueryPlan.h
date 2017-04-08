
//
// Created by 张程易 on 2017/4/8.
//

#ifndef DB_QUERYPLAN_H
#define DB_QUERYPLAN_H

#include "../Common.h"
#include "../Services/RecordService.h"

class QueryPlan{
public:
    virtual JSON * toJSON() = 0;
    virtual JSON * runQuery(
            RecordService *recordService
    ) = 0;
};


#endif //DB_QUERYPLAN_H
