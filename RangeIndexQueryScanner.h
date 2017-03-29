//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_RANGEINDEXQUERYSCANNER_H
#define DB_RANGEINDEXQUERYSCANNER_H

#include "QueryScanner.h"
#include "BPlusTree.h"

template <typename T>
class RangeIndexQueryScanner : public QueryScanner{
    int ifid;
    int tfid;
    T left, right;
    bool leq, req;
    bool withLeft, withRight;
public:
    RangeIndexQueryScanner(RecordService *recordService, BlockService *blockService, size_t len, int ifid, int tfid,
                           T left, T right, bool leq, bool req, bool withLeft, bool withRight) : QueryScanner(
            recordService, blockService, len), ifid(ifid), tfid(tfid), left(left), right(right), leq(leq), req(req),
                                                                                                 withLeft(withLeft),
                                                                                                 withRight(withRight) {}

    void scan(std::function<void(size_t, void *)> consumer) override {
        BPlusTree<T> bPlusTree(blockService, ifid);

        bPlusTree.findByRange(withLeft, left, leq, withRight, right, req,
                              [consumer, this](size_t id, size_t blk){
                                  this->recordService->read(this->tfid, blk / perBlock, blk % perBlock, len);
                              });
    }
};

#endif //DB_RANGEINDEXQUERYSCANNER_H
