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
    static JSON * toJSON(T value, int size){
        return new JSONString(std::string(value, size));
    }
    static void * toVoid(T & value){
        return (void *)value;
    }
};

template<>
class SQLConditionUtils<int>{
public:
    static JSON * toJSON(int value, int size){
        return new JSONInteger(value);
    }
    static void * toVoid(int & value){
        return (void *)&value;
    }
};

template<>
class SQLConditionUtils<double>{
public:
    static JSON * toJSON(double value, int size){
        return new JSONDouble(value);
    }
    static void * toVoid(double & value){
        return (void *)&value;
    }
};

#endif //DB_SQLCONDITIONUTILS_H
