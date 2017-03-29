//
// Created by 张程易 on 2017/3/29.
//

#include "LinearQueryScanner.h"

LinearQueryScanner::LinearQueryScanner(RecordService *recordService, BlockService *blockService, size_t len, int fid) : QueryScanner(
        recordService, blockService, len), fid(fid) {}

void LinearQueryScanner::scan(std::function<void(size_t, void *)> consumer)  {
    size_t blkCnt = blockService->getBlockCnt(fid);
    size_t cnt = 0;
    for(size_t i = 0; i < blkCnt; i++){
        BlockItem * blk = blockService->getBlock(fid, i);
        size_t offset = *(size_t *)blk->value;
        size_t now = len;
        char * ptr = (char *)blk->value + len;
        while(offset != 0){
            while( now < offset){
                consumer(++cnt, (void *)(ptr));
                ptr+=len;
                now+=len;
            }
            offset = *(size_t *)ptr;
            now += len;
            ptr += len;
        }
        while( now + len < BLOCK_SIZE){
            consumer(++cnt, (void *)(ptr));
            ptr+=len;
            now+=len;
        }
    }
}