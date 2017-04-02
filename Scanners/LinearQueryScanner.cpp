//
// Created by 张程易 on 2017/4/2.
//

#include "LinearQueryScanner.h"



LinearQueryScanner::LinearQueryScanner(RecordService *recordService,int fid,size_t len) : QueryScanner(len, recordService),
                                                                             fid(fid) {}

void LinearQueryScanner::scan(std::function<void(size_t, void *)> consumer)  {
    return recordService->scan(fid, len, consumer);
}

JSON *LinearQueryScanner::toJSON()  {
    auto json = new JSONObject();
    json->hashMap.emplace("type", new JSONString("LinearQueryScanner"));
    return json;
}

