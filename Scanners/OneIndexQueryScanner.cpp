//
// Created by 张程易 on 2017/4/2.
//
#include "OneIndexQueryScanner.h"


OneIndexQueryScanner::OneIndexQueryScanner(AbstractIndexRunner * indexRunner,RecordService * recordService, int len, int tfid,
                     void * value, std::string on) : QueryScanner(len,recordService),indexRunner(indexRunner), on(on), tfid(tfid),
                                                     value(value) {}

void OneIndexQueryScanner::scan(std::function<void(int, void *)> consumer)  {
    int ptr = indexRunner->findOne(value);
    if(ptr != 0) consumer(0, recordService->read(tfid, ptr / BLOCK_SIZE, ptr % BLOCK_SIZE, len));
}

JSON *OneIndexQueryScanner::toJSON()  {
    auto json = new JSONObject();
    json->set("type", "OneIndexQueryScanner");
    json->set("on", on);
    json->set("value", ColumnTypeUtil::toJSON(indexRunner->getType(), value));
    return json;
}
