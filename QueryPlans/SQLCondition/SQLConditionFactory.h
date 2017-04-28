//
// Created by 张程易 on 2017/4/27.
//

#ifndef DB_SQLCONDITIONFACTORY_H
#define DB_SQLCONDITIONFACTORY_H

#include "AbstractSQLCondition.h"
#include "GreaterSQLCondition.h"
#include "GreaterOrEqualSQLCondition.h"

class AbstractSQLConditionFactory{
public:
    virtual AbstractSQLCondition * createSQLCondition(
            AbstractSQLCondition::Type type,
            int size,
            int cid,
            int on,
            void * data
    ) = 0;
    virtual ~AbstractSQLConditionFactory(){}
};

template<typename T>
class SQLConditionFactory : public AbstractSQLConditionFactory{
public:
    AbstractSQLCondition *createSQLCondition(
            AbstractSQLCondition::Type type, int size, int cid, int on, void *data) override {
        switch(type){
            case AbstractSQLCondition::Type::gt:
                return new GreaterSQLCondition<T>(
                        *(T *)data,
                        cid,
                        on,
                        size
                );

            default: return nullptr;
        }
    }

    ~SQLConditionFactory() override {

    }
};

template<>
class SQLConditionFactory<char *> : public AbstractSQLConditionFactory{
public:
    AbstractSQLCondition *createSQLCondition(
            AbstractSQLCondition::Type type, int size, int cid, int on, void *data) override {
        switch(type){
            case AbstractSQLCondition::Type::gt:
                return new GreaterSQLCondition<char *>(
                        (char *)data,
                        cid,
                        on,
                        size
                );
            default: return nullptr;
        }
    }

    ~SQLConditionFactory() override {

    }
};



#endif //DB_SQLCONDITIONFACTORY_H
