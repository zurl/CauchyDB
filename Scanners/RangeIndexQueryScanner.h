//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_RANGEINDEXQUERYSCANNER_H
#define DB_RANGEINDEXQUERYSCANNER_H

#include "QueryScanner.h"
#include "../BPlusTree.h"

class RangeIndexQueryScanner : public QueryScanner{
    AbstractIndexRunner * indexRunner;
    void * left;
    void * right;
    std::string on;
    int tfid;
    bool leq, req;
    bool withLeft, withRight;
public:
    RangeIndexQueryScanner(AbstractIndexRunner * indexRunner, RecordService * recordService, int len, int tfid,
                           void * left, void * right, bool leq, bool req, bool withLeft, bool withRight, std::string on);

    void scan(std::function<void(int, void *)> consumer) override ;

    JSON *toJSON() override ;
};

#endif //DB_RANGEINDEXQUERYSCANNER_H
