//
// Created by 张程易 on 2017/4/2.
//

#include "RangeIndexQueryScanner.h"

RangeIndexQueryScanner::RangeIndexQueryScanner(AbstractIndexRunner * indexRunner, RecordService * recordService, size_t len, int tfid,
void * left, void * right, bool leq, bool req, bool withLeft, bool withRight, std::string on) : QueryScanner(
        len,recordService),indexRunner(indexRunner),  tfid(tfid), left(left), right(right), leq(leq), req(req),
withLeft(withLeft), withRight(withRight), on(on) {}

void RangeIndexQueryScanner::scan(std::function<void(size_t, void *)> consumer)  {
    indexRunner->findByRange(withLeft, left, leq, withRight, right, req,
                             [consumer, this](size_t id, size_t blk){
                                 recordService->read(tfid, blk / perBlock, blk % perBlock, len);
                             });
}

JSON *RangeIndexQueryScanner::toJSON()  {
    auto json = new JSONObject();
    json->hashMap.emplace("type", new JSONString("RangeIndexQueryScanner"));
    json->hashMap.emplace("on", new JSONString(on));
    json->hashMap.emplace("left",ColumnTypeUtil::toJSON(indexRunner->getType(), left));
    json->hashMap.emplace("right", ColumnTypeUtil::toJSON(indexRunner->getType(), right));
    json->hashMap.emplace("withLeft", new JSONBoolean(withLeft));
    json->hashMap.emplace("withRight", new JSONBoolean(withRight));
    json->hashMap.emplace("leq", new JSONBoolean(leq));
    json->hashMap.emplace("req", new JSONBoolean(req));
    return json;
}