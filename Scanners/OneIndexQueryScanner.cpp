//
// Created by 张程易 on 2017/4/2.
//
#include "OneIndexQueryScanner.h"


OneIndexQueryScanner::OneIndexQueryScanner(AbstractIndexRunner * indexRunner,RecordService * recordService, size_t len, int tfid,
                     void * value, std::string on) : QueryScanner(len,recordService),indexRunner(indexRunner), tfid(tfid),
                                                     value(value), on(on) {}

void OneIndexQueryScanner::scan(std::function<void(size_t, void *)> consumer)  {
    size_t ptr = indexRunner->findOne(value);
    if(ptr != 0) consumer(0, recordService->read(tfid, ptr / perBlock, ptr % perBlock, len));
}

JSON *OneIndexQueryScanner::toJSON()  {
    auto json = new JSONObject();
    json->set("type", "OneIndexQueryScanner");
    json->set("on", on);
    json->set("value", ColumnTypeUtil::toJSON(indexRunner->getType(), value));
    return json;
}
