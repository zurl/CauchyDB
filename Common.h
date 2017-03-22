//
// Created by 张程易 on 2017/3/20.
//

#ifndef DB_COMMON_H
#define DB_COMMON_H

#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <list>
#include <vector>
#include <ctime>
#include <cassert>
#include <map>
#include "json.h"

const int BLOCK_SIZE = 4 + sizeof(size_t) + (16 + sizeof(size_t)) * 5;

struct BlockItem{
    int fid;
    size_t offset;
    char flag;
    bool modified;
    void * value;
};

#endif //DB_COMMON_H
