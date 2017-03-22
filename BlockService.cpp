//
// Created by 张程易 on 2017/3/20.
//
#include "BlockService.h"

void BlockService::cleanup(){
    // using sample stragegy
    printf("trigger block clean up...\n");
    auto iter = accessQueue.begin();
    while( iter != accessQueue.end()){
        auto item = *iter;
        if( item->modified ){
            printf("write back at fid = %d, blk = %d\n", item->fid, (int) item->offset);
            fileService->writeBlock(item->fid, item->offset, item->value);
        }
        delete item;
        iter = accessQueue.erase(iter);
        blockCnt --;
    }
    if( accessQueue.size() <= MAX_BLOCK_CACHE / 3 ){
        iter = cacheQueue.begin();
        while( iter != cacheQueue.end()){
            auto item = *iter;
            if( item->modified ){
                printf("write back at fid = %d, blk = %d\n", item->fid, (int) item->offset);
                fileService->writeBlock(item->fid, item->offset, item->value);
            }
            delete item;
            iter = cacheQueue.erase(iter);
            blockCnt --;
        }
    }
}
BlockService::BlockService(FileService * fileService)
        :fileService(fileService){}
BlockService::~BlockService(){
    auto iter = accessQueue.begin();
    while( iter != accessQueue.end()){
        auto item = *iter;
        if( item->modified ){
            printf("[~] write back at fid = %d, blk = %d\n", item->fid, (int) item->offset);
            fileService->writeBlock(item->fid, item->offset, item->value);
        }
        delete item;
        iter = accessQueue.erase(iter);
        blockCnt --;
    }
    iter = cacheQueue.begin();
    while( iter != cacheQueue.end()){
        auto item = *iter;
        if( item->modified ){
            printf("[~] write back at fid = %d, blk = %d\n", item->fid, (int) item->offset);
            fileService->writeBlock(item->fid, item->offset, item->value);
        }
        delete item;
        iter = cacheQueue.erase(iter);
        blockCnt --;
    }
}
size_t BlockService::allocBlock(int fid) {
    return fileService->allocBlock(fid);
}
size_t BlockService::getBlockCnt(int fid){
    return fileService->getBlockCnt(fid);
}
BlockItem * BlockService::getBlock(int fid, size_t offset){
    //printf("get %d %d\n", fid, offset);
    auto iter = cacheQueue.begin();
    while( iter != cacheQueue.end()){
        auto item = *iter;
        if( item->fid == fid && item->offset == offset){
            item->flag = (char)time(NULL);
            cacheQueue.push_front(item);
            cacheQueue.erase(iter);
            return item;
        }
        iter++;
    }
    iter = accessQueue.begin();
    while( iter != accessQueue.end()) {
        auto item = *iter;
        //printf("@@ item fid=%d blk=%d v[0]=%s v[1]=%s\n",item->fid, item->offset, tmp->v[0], tmp->v[1]);
        if( item->fid == fid && item->offset == offset){
            item->flag++;
            if( item->flag == 2){
                cacheQueue.push_front(item);
                accessQueue.erase(iter);
            }
            else{
                accessQueue.push_front(item);
                accessQueue.erase(iter);
            }
            //printf("return item with v[0]=%s v[1]=%s\n", tmp->v[0], tmp->v[1]);
            return item;
        }
        iter++;
    }
    if( blockCnt >= MAX_BLOCK_CACHE){
        cleanup();
    }
    blockCnt ++;
    BlockItem * item = new BlockItem();
    item->fid = fid;
    item->offset = offset;
    item->modified = 0;
    item->flag = 0;
    item->value = fileService->readBlock(fid, offset);
    accessQueue.push_front(item);
    printf("load block at fid = %d, blk = %d\n", item->fid, (int) item->offset);
    return item;
}
