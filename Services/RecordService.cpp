//
// Created by 张程易 on 2017/3/22.
//

#include "RecordService.h"

RecordService::RecordService(BlockService * blockService)
        :blockService(blockService){}

void * RecordService::read(int fid, size_t block, size_t offset, size_t len){
    BlockItem * blockItem = blockService->getBlock(fid, block);
    return (void *)((char *)blockItem->value + offset);
}
void RecordService::write(int fid, size_t block, size_t offset, void * data, size_t len){
    assert( len >= 8 );
    BlockItem * blockItem = blockService->getBlock(fid, block);
    memcpy((char *)blockItem->value + offset, data, len);
    blockItem->modified = 1;
}
size_t RecordService::insertIntoNewBlock(int fid, void * data, size_t len){
    size_t perBlock = BLOCK_SIZE / len;
    size_t offset = blockService->allocBlock(fid);
    BlockItem * blockItem = blockService->getBlock(fid, offset);
    char * current = (char *)blockItem->value;
    *(size_t *)current = 2 * len; // v[0] = 2;
    current += len; // current = 1;
    memcpy(current, data, len);
    current += len;
    for(size_t i = 3 * len; i < perBlock * len ; i+= len){
        *(size_t *)current = i ;
        current += len;
    }
    *(size_t *)current = 0;
    blockItem->modified = 1;
    return offset * BLOCK_SIZE + len;
}
size_t RecordService::insert(int fid, void * data, size_t len){
    assert( len >= 8 );
    size_t offset = blockService->getBlockCnt(fid);
    if( offset == 0){
        return insertIntoNewBlock(fid, data, len);
    }
    else{
        offset--;
        BlockItem * blockItem = blockService->getBlock(fid, offset);
        char * current = (char *)blockItem->value;
        size_t location = *(size_t *)current;
        if( location == 0 ){
            return insertIntoNewBlock(fid, data, len);
        }
        char * target = current + location;
        *(size_t *)current = *(size_t *)target;
        memcpy(target, data, len);
        blockItem->modified = 1;
        return offset * BLOCK_SIZE + location;
    }
}
void RecordService::remove(int fid, size_t block, size_t offset, size_t len){
    assert( len >= 8 );
    BlockItem * blockItem = blockService->getBlock(fid, block);
    char * current = (char *)blockItem->value;
    char * target = current + offset;
    *(size_t *)target = *(size_t *)current;
    *(size_t *)current = offset;
}

void RecordService::scan(int fid, size_t len, std::function<void(size_t, void *)> consumer)  {
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