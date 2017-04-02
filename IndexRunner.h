//
// Created by 张程易 on 2017/4/2.
//

#ifndef DB_INDEXEXCUTER_H
#define DB_INDEXEXCUTER_H

#include "AbstractIndexRunner.h"
#include "BPlusTree.h"

template<typename T>
class IndexRunnerWrapper{
public:
    static char * convert(void * data){
        return (char *)data;
    }
    static ColumnType getType(){
        return ColumnType::Char;
    }
};

template<>
class IndexRunnerWrapper<int>{
public:
    static int convert(void * data){
        return *(int *)data;
    }
    static ColumnType getType(){
        return ColumnType::Int;
    }
};

template<>
class IndexRunnerWrapper<double>{
public:
    static double convert(void * data){
        return *(double *)data;
    }
    static ColumnType getType(){
        return ColumnType::Float;
    }
};

template<typename T>
class IndexRunner: public AbstractIndexRunner{
    BPlusTree<T> bPlusTree;
public:
    IndexRunner(int fid, BlockService *blockService) : bPlusTree(blockService, fid){}

    int insert(void * key, size_t value) override {
        bPlusTree.insert(IndexRunnerWrapper<T>::convert(key), value);
        return 1;
    }

    size_t findOne(void * key){
        return bPlusTree.findOne(IndexRunnerWrapper<T>::convert(key));
    }

    size_t findByRange(
            bool withLeft, void * left, bool leftEqu,
            bool withRight, void * right, bool rightEqu,
            std::function<void(size_t, size_t)> consumer
    ){
        return bPlusTree.findByRange(
                withLeft, IndexRunnerWrapper<T>::convert(left), leftEqu,
                withRight, IndexRunnerWrapper<T>::convert(right), rightEqu,
                consumer
        );
    }

    virtual ColumnType getType() override {
        return IndexRunnerWrapper<T>::getType();
    }
};

#endif //DB_INDEXEXCUTER_H
