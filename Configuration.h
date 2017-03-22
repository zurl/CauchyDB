//
// Created by 张程易 on 2017/3/22.
//

#ifndef DB_CONFIGURATION_H
#define DB_CONFIGURATION_H
#include "Common.h"


class Configuration{
    static JSON * config;
public:
    static void initialize();
    static const char * attrCString(const char * key);
    static JSON * getJSON();
};


#endif //DB_CONFIGURATION_H
