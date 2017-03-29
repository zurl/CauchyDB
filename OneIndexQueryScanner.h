//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_ONEINDEXQUERYSCANNER_H
#define DB_ONEINDEXQUERYSCANNER_H
#include "QueryScanner.h"
#include "BPlusTree.h"

template <typename T>
class OneIndexQueryScanner : public QueryScanner{
    int ifid;
    int tfid;
    T value;
public:
    OneIndexQueryScanner(RecordService *recordService, BlockService *blockService, size_t len, int ifid, int tfid,
                         T value) : QueryScanner(recordService, blockService, len), ifid(ifid), tfid(tfid),
                                    value(value) {}

    void scan(std::function<void(size_t, void *)> consumer) override {
        BPlusTree<T> bPlusTree(blockService, ifid);
        size_t ptr = bPlusTree.findOne(value);
        if(ptr != 0) consumer(0, recordService->read(tfid, ptr / perBlock, ptr % perBlock, len));
    }
};



#endif //DB_ONEINDEXQUERYSCANNER_H
