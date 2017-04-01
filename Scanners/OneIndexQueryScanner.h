//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_ONEINDEXQUERYSCANNER_H
#define DB_ONEINDEXQUERYSCANNER_H
#include "QueryScanner.h"
#include "../BPlusTree.h"

template <typename T>
class OneIndexQueryScanner : public QueryScanner{
    std::string on;
    int ifid;
    int tfid;
    typename TypeUtil<T>::ref_type value;
public:
    OneIndexQueryScanner(RecordService *recordService, BlockService *blockService, size_t len, int ifid, int tfid,
                         T value, std::string on) : QueryScanner(recordService, blockService, len), ifid(ifid), tfid(tfid),
                                    value(value), on(on) {}

    void scan(std::function<void(size_t, void *)> consumer) override {
        BPlusTree<T> bPlusTree(blockService, ifid);
        size_t ptr = bPlusTree.findOne(value);
        if(ptr != 0) consumer(0, recordService->read(tfid, ptr / perBlock, ptr % perBlock, len));
    }

    JSON *toJSON() override {
        auto json = new JSONObject();
        json->hashMap.emplace("type", new JSONString("OneIndexQueryScanner"));
        json->hashMap.emplace("on", new JSONString(on));
        json->hashMap.emplace("value", TypeUtil<T>::toJSON(value));
        return json;
    }
};



#endif //DB_ONEINDEXQUERYSCANNER_H