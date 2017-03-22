//
// Created by 张程易 on 2017/3/22.
//

#ifndef DB_QUERYFILTER_H
#define DB_QUERYFILTER_H

#include "BPlusTree.h"

class QueryFilter{
public:
    virtual bool filter(void * data) = 0;
};

class AndQueryFilter: public QueryFilter{
    std::vector<QueryFilter *> filters;
public:
    void addFilter(QueryFilter * filter){
        filters.emplace_back(filter);
    }
    bool filter(void *data) override {
        for(auto & filter : filters){
            if(!filter->filter(data))return false;
        }
        return true;
    }
};

template<typename T>
class CompareQueryFilter: public QueryFilter{
protected:
    int st;
    T val;
public:
    CompareQueryFilter(int st, T val) : st(st) {
        strcpy(this->val, val);
    }
    virtual bool filter(void * data) = 0;
};

template<>
class CompareQueryFilter<int>: public QueryFilter{
protected:
    int st;
    int val;
public:
    CompareQueryFilter(int st, int val) : st(st), val(val) {}
    virtual bool filter(void * data) = 0;
};

template<>
class CompareQueryFilter<double>: public QueryFilter{
protected:
    int st;
    double val;
public:
    CompareQueryFilter(int st, double val) : st(st), val(val) {}
    virtual bool filter(void * data) = 0;
};

template<typename T>
class GreaterQueryFilter : public CompareQueryFilter<T>{
public:
    GreaterQueryFilter(int st, T val): CompareQueryFilter(st, val){}
    bool filter(void *data) override {
        return TypeUtil<T>::cmp(this->val, (T *)((char *)data + st)) > 0;
    }
};

template<typename T>
class EqualQueryFilter : public CompareQueryFilter<T>{
public:
    EqualQueryFilter(int st, T val): CompareQueryFilter(st, val){}
    bool filter(void *data) override {
        return TypeUtil<T>::cmp(this->val, (T *)((char *)data + st)) == 0;
    }
};

template<typename T>
class GreaterOrEqualQueryFilter : public CompareQueryFilter<T>{
public:
    GreaterOrEqualQueryFilter(int st, T val): CompareQueryFilter(st, val){}
    bool filter(void *data) override {
        return TypeUtil<T>::cmp(this->val, (T *)((char *)data + st)) >= 0;
    }
};

#endif //DB_QUERYFILTER_H
