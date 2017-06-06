//
// Created by 张程易 on 2017/4/2.
//

#include "RangeIndexQueryScanner.h"

RangeIndexQueryScanner::RangeIndexQueryScanner(AbstractIndexRunner * indexRunner, RecordService * recordService, int len, int tfid,
void * left, void * right, bool leq, bool req, bool withLeft, bool withRight, std::string on) : QueryScanner(
        len,recordService),indexRunner(indexRunner), left(left), right(right),on(on),tfid(tfid),   leq(leq), req(req),
                                                                                                withLeft(withLeft), withRight(withRight) {}

void RangeIndexQueryScanner::scan(std::function<void(int, void *)> consumer)  {
    indexRunner->findByRange(withLeft, left, leq, withRight, right, req,
                             [consumer, this](int id, int blk){
                                 recordService->read(tfid, blk / BLOCK_SIZE, blk % BLOCK_SIZE, len);
                             });
}

JSON *RangeIndexQueryScanner::toJSON()  {
    auto json = new JSONObject();
    json->set("type", "RangeIndexQueryScanner");
    json->set("on", on);
    json->set("left",ColumnTypeUtil::toJSON(indexRunner->getType(), left));
    json->set("right", ColumnTypeUtil::toJSON(indexRunner->getType(), right));
    json->set("withLeft", withLeft);
    json->set("withRight", withRight);
    json->set("leq", leq);
    json->set("req", req);
    return json;
}