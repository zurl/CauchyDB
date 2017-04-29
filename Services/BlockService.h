//
// Created by 张程易 on 2017/3/20.
//

#ifndef DB_BLOCKSERVICE_H
#define DB_BLOCKSERVICE_H
#include "FileService.h"
#include "../Common.h"



class BlockService{ // using LRUk Algoirthm
    const int MAX_BLOCK_CACHE = 4096; //64 * 4kb = 256kb
    const int LRU_K_VALUE = 2;
    FileService * fileService;
    std::list<BlockItem *> accessQueue;
    std::list<BlockItem *> cacheQueue;
    int blockCnt = 0;
    void cleanup();
public:
    BlockService(FileService * fileService);
    ~BlockService();
    int allocBlock(int fid);
    int getBlockCnt(int fid);
    BlockItem * getBlock(int fid, int offset);
};

#endif //DB_BLOCKSERVICE_H
