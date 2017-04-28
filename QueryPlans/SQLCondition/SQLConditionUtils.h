//
// Created by 张程易 on 2017/4/27.
//

#ifndef DB_SQLCONDITIONUTILS_H
#define DB_SQLCONDITIONUTILS_H

#include "../../Common.h"
#include "AbstractSQLCondition.h"

template<typename T>
class SQLConditionUtils{
public:
    inline static JSON * toJSON(T value, int size){
        return new JSONString(std::string(value, size));
    }
    inline static void * toVoid(T & value){
        return (void *)value;
    }
    inline static char * extract(void * data){
        return (char *)data;
    }
};

template<>
class SQLConditionUtils<int>{
public:
    inline static JSON * toJSON(int value, int size){
        return new JSONInteger(value);
    }
    inline static void * toVoid(int & value){
        return (void *)&value;
    }
    inline static int extract(void * data){
        return *(int *)data;
    }
};

template<>
class SQLConditionUtils<double>{
public:
    inline static JSON * toJSON(double value, int size){
        return new JSONDouble(value);
    }
    inline static void * toVoid(double & value){
        return (void *)&value;
    }
    inline static double extract(void * data){
        return *(double *)data;
    }
};

#endif //DB_SQLCONDITIONUTILS_H
