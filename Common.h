//
// Created by 张程易 on 2017/3/20.
//

#ifndef DB_COMMON_H
#define DB_COMMON_H

#define CAUCHY_DEBUG1

#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <list>
#include <functional>
#include <vector>
#include <ctime>
#include <cassert>
#include <map>
#include "JSON/JSON.h"
#include "Exception.h"

const int BLOCK_SIZE = 8192 * 4;

namespace Util {
    inline int stoi(const std::string &str) {
        std::istringstream is(str);
        int i;
        is >> i;
        return i;
    }

    inline std::string itos(int i) {
        std::ostringstream os;
        os << i;
        return os.str();
    }
}

struct BlockItem{
    int fid;
    int offset;
    char flag;
    bool modified;
    void * value;
};

class JSONMessage{
    int status;
    std::string message;
public:
    inline JSONMessage(int status, const std::string &message) : status(status), message(message) {}
    inline JSON * toJSON(){
        auto json = new JSONObject();
        json->set("status", status);
        json->set("message", message);
        return json;
    }
};


template<typename Type>
class TypeUtil {
public:
    using ref_type = char *;
    using Conv = const char *;
    static void set(Type &a, const Type &b){
        strcpy(a, b);
    }
    static void set(Type &a, const char * b){
        strcpy(a, b);
    }

    static int cmp(const Type a, const Type b){
        int ret = strcmp((const char *)a, (const char *)b);
        if ( ret > 0) return 1;
        if ( ret == 0) return 0;
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
    static void set(int &a, const int &b){
        a = b;
    }
    static int cmp(const int & a, const int & b){
        if( a < b ) return -1;
        if( a > b ) return 1;
        return 0;
    }
    static JSON * toJSON(int v){
        return new JSONInteger(v);
    }
    using ref_type = int;
};

template<>
class TypeUtil<double>{
public:
    static void set(double &a, const double &b){
        a = b;
    }
    static int cmp(const double & a, const double & b){
        if( a < b ) return -1;
        if( a > b ) return 1;
        return 0;
    }
    static JSON * toJSON(double v){
        return new JSONDouble(v);
    }
    using ref_type = double;
};

enum class ColumnType{
    Char, Int, Float
};

class ColumnTypeUtil{
public:
    static std::string toString(ColumnType type){
        if( type == ColumnType::Char ){
            return "char";
        }
        else if( type == ColumnType::Int){
            return "int";
        }
        else {
            return "double";
        }
    }
    static JSON * toJSON(ColumnType type, void * data){
        if( data == nullptr) return new JSONNull();
        if( type == ColumnType::Char ){
            return new JSONString((char *)data);
        }
        else if( type == ColumnType::Int){
            return new JSONInteger(*(int *)data);
        }
        else {
            return new JSONDouble(*(double *)data);
        }
    }
};



#endif //DB_COMMON_H
