//
// Created by 张程易 on 2017/4/26.
//

#ifndef DB_GREATERSQLCONDITION_H
#define DB_GREATERSQLCONDITION_H

#include "BasicSQLCondition.h"

template<typename T>
class GreaterSQLCondition{

};

template<>
class GreaterSQLCondition<char *> : public BasicSQLCondition<char *>{
public:
    GreaterSQLCondition(char * value, int on, int size) : BasicSQLCondition(value, on, size) {}
    bool filter(void *data) override {
        char * str = (char *) data;
        for(int i = 0; i < size; i++){
            if(str[i] > value[i]) return true;
            if(str[i] < value[i]) return false;
        }
        return false;
    }
};

template<>
class GreaterSQLCondition<char> : public BasicSQLCondition<int>{
public:
    GreaterSQLCondition(int data, int on, int size) : BasicSQLCondition(value, on, size) {}
    bool filter(void *data) override {
        return *(int *)data > value;
    }
};

#endif //DB_GREATERSQLCONDITION_H
