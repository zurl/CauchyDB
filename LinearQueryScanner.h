//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_LINEARQUERYSCANNER_H
#define DB_LINEARQUERYSCANNER_H

#include "QueryScanner.h"

class LinearQueryScanner : public QueryScanner{
    int fid;
public:
    LinearQueryScanner(RecordService *recordService, BlockService *blockService, size_t len, int fid);

    void scan(std::function<void(size_t, void *)> consumer) override ;

    JSON *toJSON() override;
};


#endif //DB_LINEARQUERYSCANNER_H
