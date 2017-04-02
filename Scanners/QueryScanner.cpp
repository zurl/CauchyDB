//
// Created by 张程易 on 2017/3/29.
//

#include "QueryScanner.h"

QueryScanner::QueryScanner(size_t len, RecordService *recordService) : len(len), recordService(recordService) {
    perBlock = BLOCK_SIZE / len;
}
