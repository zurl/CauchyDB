//
// Created by 张程易 on 2017/3/22.
//

#include "RecordService.h"

RecordService::RecordService(BlockService * blockService)
        :blockService(blockService){}

void * RecordService::read(int fid, int block, int offset, int len){
    BlockItem * blockItem = blockService->getBlock(fid, block);
    return (void *)((char *)blockItem->value + offset);
}
void RecordService::write(int fid, int block, int offset, void * data, int len){
    assert( len >= 8 );
    BlockItem * blockItem = blockService->getBlock(fid, block);
    memcpy((char *)blockItem->value + offset, data, len);
    blockItem->modified = 1;
}
int RecordService::insertIntoNewBlock(int fid, void * data, int len){
    int perBlock = BLOCK_SIZE / len;
    int offset = blockService->allocBlock(fid);
    BlockItem * blockItem = blockService->getBlock(fid, offset);
    char * current = (char *)blockItem->value;
    *(int *)current = 2 * len; // v[0] = 2;
    current += len; // current = 1;
    memcpy(current, data, len);
    current += len;
    for(int i = 3 * len; i < perBlock * len ; i+= len){
        *(int *)current = i ;
        current += len;
    }
    *(int *)current = 0;
    blockItem->modified = 1;
    return offset * BLOCK_SIZE + len;
}
int RecordService::insert(int fid, void * data, int len){
    assert( len >= 8 );
    int offset = blockService->getBlockCnt(fid);
    if( offset == 0){
        return insertIntoNewBlock(fid, data, len);
    }
    else{
        offset--;
        BlockItem * blockItem = blockService->getBlock(fid, offset);
        char * current = (char *)blockItem->value;
        int location = *(int *)current;
        if( location == 0 ){
            return insertIntoNewBlock(fid, data, len);
        }
        char * target = current + location;
        *(int *)current = *(int *)target;
        memcpy(target, data, len);
        blockItem->modified = 1;
        return offset * BLOCK_SIZE + location;
    }
}
void RecordService::remove(int fid, int block, int offset, int len){
    assert( len >= 8 );
    BlockItem * blockItem = blockService->getBlock(fid, block);
    char * current = (char *)blockItem->value;
    char * target = current + offset;
    *(int *)target = *(int *)current;
    *(int *)current = offset;
}

void RecordService::scan(int fid, int len, std::function<void(int, void *)> consumer)  {
    int blkCnt = blockService->getBlockCnt(fid);
    int cnt = 0;
    for(int i = 0; i < blkCnt; i++){
        BlockItem * blk = blockService->getBlock(fid, i);
        int offset = *(int *)blk->value;
        int now = len;
        char * ptr = (char *)blk->value + len;
        while(offset != 0){
            while( now < offset){
                consumer(++cnt, (void *)(ptr));
                ptr+=len;
                now+=len;
            }
            offset = *(int *)ptr;
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