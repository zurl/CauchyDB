//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_LINEARQUERYSCANNER_H
#define DB_LINEARQUERYSCANNER_H

#include "QueryScanner.h"

class LinearQueryScanner : public QueryScanner{
    int fid;
public:
    LinearQueryScanner(RecordService *recordService,int fid,int len);

    void scan(std::function<bool(int, void *)> consumer) override;

    JSON *toJSON() override;
};


#endif //DB_LINEARQUERYSCANNER_H
