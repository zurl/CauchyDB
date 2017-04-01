//
// Created by 张程易 on 2017/3/22.
//

#ifndef DB_RECORDSERVICE_H
#define DB_RECORDSERVICE_H
#include "../Common.h"
#include "BlockService.h"

class RecordService{
    BlockService * blockService;
public:
    RecordService(BlockService * blockService);
    void * read(int fid, size_t block, size_t offset, size_t len);
    void write(int fid, size_t block, size_t offset, void * data, size_t len);
    size_t insertIntoNewBlock(int fid, void * data, size_t len);
    size_t insert(int fid, void * data, size_t len);
    void remove(int fid, size_t block, size_t offset, size_t len);
};


#endif //DB_RECORDSERVICE_H
