//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_RANGEINDEXQUERYSCANNER_H
#define DB_RANGEINDEXQUERYSCANNER_H

#include "QueryScanner.h"
#include "../BPlusTree.h"

template <typename T>
class RangeIndexQueryScanner : public QueryScanner{

    typename TypeUtil<T>::ref_type left;
    typename TypeUtil<T>::ref_type right;
    std::string on;
    int ifid;
    int tfid;
    bool leq, req;
    bool withLeft, withRight;
public:
    RangeIndexQueryScanner(RecordService *recordService, BlockService *blockService, size_t len, int ifid, int tfid,
                           T left, T right, bool leq, bool req, bool withLeft, bool withRight, std::string on) : QueryScanner(
            recordService, blockService, len), ifid(ifid), tfid(tfid), left(left), right(right), leq(leq), req(req),
                                                                                                 withLeft(withLeft), withRight(withRight), on(on) {}

    void scan(std::function<void(size_t, void *)> consumer) override {
        BPlusTree<T> bPlusTree(blockService, ifid);

        bPlusTree.findByRange(withLeft, left, leq, withRight, right, req,
                              [consumer, this](size_t id, size_t blk){
                                  this->recordService->read(this->tfid, blk / perBlock, blk % perBlock, len);
                              });
    }

    JSON *toJSON() override {
        auto json = new JSONObject();
        json->hashMap.emplace("type", new JSONString("RangeIndexQueryScanner"));
        json->hashMap.emplace("on", new JSONString(on));
        json->hashMap.emplace("left", TypeUtil<T>::toJSON(left));
        json->hashMap.emplace("right", TypeUtil<T>::toJSON(right));
        json->hashMap.emplace("withLeft", new JSONBoolean(withLeft));
        json->hashMap.emplace("withRight", new JSONBoolean(withRight));
        json->hashMap.emplace("leq", new JSONBoolean(leq));
        json->hashMap.emplace("req", new JSONBoolean(req));
        return json;
    }
};

#endif //DB_RANGEINDEXQUERYSCANNER_H
