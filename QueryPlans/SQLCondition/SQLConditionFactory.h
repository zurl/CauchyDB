//
// Created by 张程易 on 2017/4/27.
//

#ifndef DB_SQLCONDITIONFACTORY_H
#define DB_SQLCONDITIONFACTORY_H

#include "AbstractSQLCondition.h"
#include "GreaterSQLCondition.h"
#include "EqualSQLCondition.h"
#include "NotEqualSQLCondition.h"
#include "LessOrEqualSQLCondition.h"
#include "LessSQLCondition.h"
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
                        SQLConditionUtils<T>::extract(data),
                        cid,
                        on,
                        size
                );
            case AbstractSQLCondition::Type::neq:
                return new NotEqualSQLCondition<T>(
                        SQLConditionUtils<T>::extract(data),
                        cid,
                        on,
                        size
                );
            case AbstractSQLCondition::Type::lte:
                return new LessOrEqualSQLCondition<T>(
                        SQLConditionUtils<T>::extract(data),
                        cid,
                        on,
                        size
                );
            case AbstractSQLCondition::Type::lt:
                return new LessSQLCondition<T>(
                        SQLConditionUtils<T>::extract(data),
                        cid,
                        on,
                        size
                );
            case AbstractSQLCondition::Type::gte:
                return new GreaterOrEqualSQLCondition<T>(
                        SQLConditionUtils<T>::extract(data),
                        cid,
                        on,
                        size
                );
            case AbstractSQLCondition::Type::eq:
                return new EqualSQLCondition<T>(
                        SQLConditionUtils<T>::extract(data),
                        cid,
                        on,
                        size
                );
        }
    }

    ~SQLConditionFactory() override {

    }
};


#endif //DB_SQLCONDITIONFACTORY_H
