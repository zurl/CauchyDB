//
// Created by 张程易 on 2017/3/29.
//

#include "QueryScanner.h"

QueryScanner::QueryScanner(RecordService *recordService, BlockService *blockService, size_t len) : recordService(recordService),
                                                                                     blockService(blockService),
                                                                                     len(len) {
    perBlock = BLOCK_SIZE / len;
}
