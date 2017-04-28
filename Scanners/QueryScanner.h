//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_QUERYSCANNER_H
#define DB_QUERYSCANNER_H

#include "../Common.h"
#include "../Services/RecordService.h"
#include "../Models/TableModel.h"

class QueryScanner{
protected:
    int len;
    RecordService * recordService;
public:
    QueryScanner(int len, RecordService *recordService);
    inline virtual ~QueryScanner(){};
    virtual void scan(std::function<void(int, void *)> consumer) = 0;
    virtual JSON * toJSON() = 0;
};

#endif //DB_QUERYSCANNER_H
