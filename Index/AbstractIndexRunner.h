//
// Created by 张程易 on 2017/4/2.
//

#ifndef DB_ABSTRACTINDEXEXCUTER_H
#define DB_ABSTRACTINDEXEXCUTER_H

#include "../Services/BlockService.h"

class AbstractIndexRunner{
public:
    virtual void initialize() = 0;
    virtual ColumnType getType() = 0;
    virtual int insert(void * key, int value) = 0;
    virtual int findOne(void * key) = 0;
    virtual int findByRange(
            bool withLeft, void * left, bool leftEqu,
            bool withRight, void * right, bool rightEqu,
            std::function<void(int, int)> consumer
    ) = 0;
    virtual ~AbstractIndexRunner() {

    }
};

#endif //DB_ABSTRACTINDEXEXCUTER_H
