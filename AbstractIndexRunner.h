//
// Created by 张程易 on 2017/4/2.
//

#ifndef DB_ABSTRACTINDEXEXCUTER_H
#define DB_ABSTRACTINDEXEXCUTER_H

#include "Services/BlockService.h"

class AbstractIndexRunner{
public:
    virtual void initialize() = 0;
    virtual ColumnType getType() = 0;
    virtual int insert(void * key, size_t value) = 0;
    virtual size_t findOne(void * key) = 0;
    virtual size_t findByRange(
            bool withLeft, void * left, bool leftEqu,
            bool withRight, void * right, bool rightEqu,
            std::function<void(size_t, size_t)> consumer
    ) = 0;
};

#endif //DB_ABSTRACTINDEXEXCUTER_H
