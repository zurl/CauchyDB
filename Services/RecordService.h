//
// Created by 张程易 on 2017/3/22.
//

#ifndef DB_RECORDSERVICE_H
#define DB_RECORDSERVICE_H
#include "../Common.h"
#include "BlockService.h"

class RecordService {
    BlockService *blockService;
public:
    RecordService(BlockService *blockService);

    void *read(int fid, int block, int offset, int len);

    void write(int fid, int block, int offset, void *data, int len);

    int insertIntoNewBlock(int fid, void *data, int len);

    int insert(int fid, void *data, int len);

    void remove(int fid, int block, int offset, int len);

    void scan(int fid, int len, std::function<void(int, void *)> consumer);
};


#endif //DB_RECORDSERVICE_H
