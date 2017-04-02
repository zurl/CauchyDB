//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_ONEINDEXQUERYSCANNER_H
#define DB_ONEINDEXQUERYSCANNER_H
#include "QueryScanner.h"
#include "../BPlusTree.h"

class OneIndexQueryScanner : public QueryScanner{
    AbstractIndexRunner * indexRunner;
    std::string on;
    int tfid;
    void * value;
public:
    OneIndexQueryScanner(AbstractIndexRunner * indexRunner,RecordService * recordService, size_t len, int tfid,
                         void * value, std::string on) ;
    void scan(std::function<void(size_t, void *)> consumer) override ;
    JSON *toJSON() override ;
};



#endif //DB_ONEINDEXQUERYSCANNER_H
