//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_QUERYSCANNER_H
#define DB_QUERYSCANNER_H

#include "Common.h"
#include "RecordService.h"

class QueryScanner{
protected:
    RecordService * recordService;
    BlockService * blockService;
    size_t len;
    size_t perBlock;
public:
    QueryScanner(RecordService *recordService, BlockService *blockService, size_t len) ;
    virtual void scan( std::function<void(size_t, void *)> consumer) = 0;
};

#endif //DB_QUERYSCANNER_H
