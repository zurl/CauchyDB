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
#include "JSON.h"
const int BLOCK_SIZE = 4 + sizeof(size_t) + (16 + sizeof(size_t)) * 5;

struct BlockItem{
    int fid;
    size_t offset;
    char flag;
    bool modified;
    void * value;
};


class SQLException{
public:
    int code;
    std::string message;
    inline SQLException(int code = 0, std::string && message = "")
            :code(code), message(message){};
};

class SQLExecuteException: public SQLException{
public:
    inline SQLExecuteException(int code = 0, std::string && message = "")
            :SQLException(code, std::move(message)){}

};

class SQLSyntaxException: public SQLException{
public:
    inline SQLSyntaxException(int code = 0, std::string && message = "")
            :SQLException(code, std::move(message)){}
};

class SQLTypeException: public SQLException{
public:
    inline SQLTypeException(int code = 0, std::string && message = "")
            :SQLException(code, std::move(message)){}
};

template<typename Type>
class TypeUtil {
public:
    using ref_type = char *;
    using Conv = const char *;
    static void set(Type &a, Type &b){
        strcpy(a, b);
    }
    static void set(Type &a, const char * b){
        strcpy(a, b);
    }
    static int cmp(Type a, Type b){
        int ret = strcmp((const char *)a, (const char *)b);
        if ( ret > 0) return 1;
        if ( ret == 0) return 0;
        return -1;
    }
    static int cmp(Type a, const char * b) {
        int ret = strcmp((const char *) a, b);
        if (ret > 0) return 1;
        if (ret == 0) return 0;
        return -1;
    }
    static JSON * toJSON(Type v){
        if( v == nullptr )return new JSONNull();
        return new JSONString(v);
    }
    static void initial(Type & dst, Type & src){
        memcpy(dst, src, sizeof(Type));
    }
};

template<>
class TypeUtil<int>{
public:
    static int cmp(int & a, int & b){
        if( a < b ) return 1;
        if( a > b ) return -1;
        return 0;
    }
    static JSON * toJSON(int v){
        return new JSONInteger(v);
    }
    using ref_type = int;
    static void initial(int & dst, int & src){
        dst = src;
    }
};

template<>
class TypeUtil<double>{
public:
    static int cmp(double & a, double & b){
        if( a < b ) return 1;
        if( a > b ) return -1;
        return 0;
    }
    static JSON * toJSON(double v){
        return new JSONDouble(v);
    }
    using ref_type = double;
    static void initial(double & dst, double & src){
        dst = src;
    }
};


#endif //DB_COMMON_H
