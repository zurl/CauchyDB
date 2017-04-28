cmake_minimum_required(VERSION 3.6)
project(db)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp
        JSON/JSON.cpp
        JSON/JSON.h
        JSON/JSONParser.cpp
        BPlusTree.h
        Common.h
        Services/BlockService.cpp
        Services/BlockService.h
        Services/FileService.cpp
        Services/FileService.h
        SQLParser.cpp SQLParser.h
        Configuration.cpp
        Configuration.h
        Services/RecordService.cpp
        Services/RecordService.h
        QueryFilter.h
        Scanners/QueryScanner.cpp
        Scanners/QueryScanner.h
        Scanners/LinearQueryScanner.h
        Scanners/RangeIndexQueryScanner.h
        Scanners/OneIndexQueryScanner.h
        Models/ColumnModel.cpp
        Models/ColumnModel.h
        Models/TableModel.cpp
        Models/TableModel.h
        Models/IndexModel.cpp
        Models/IndexModel.h
        AbstractIndexRunner.h
        IndexRunner.h
        Scanners/RangeIndexQueryScanner.cpp
        Scanners/OneIndexQueryScanner.cpp
        Scanners/LinearQueryScanner.cpp
        SQLSession.cpp SQLSession.h
        Services/MetaDataService.cpp
        Services/MetaDataService.h
        Models/DataBaseModel.cpp
        Models/DataBaseModel.h
        QueryPlans/QueryPlan.h
        QueryPlans/InsertQueryPlan.cpp
        QueryPlans/InsertQueryPlan.h
        QueryPlans/SelectQueryPlan.cpp
        QueryPlans/SelectQueryPlan.h
        QueryPlans/SQLWhereClause.cpp
        QueryPlans/SQLWhereClause.h
        QueryPlans/CreateQueryPlan.h
        QueryPlans/CreateTableQueryPlan.cpp
        QueryPlans/CreateTableQueryPlan.h
        QueryPlans/CreateDataBaseQueryPlan.cpp
        QueryPlans/CreateDataBaseQueryPlan.h QueryPlans/InterpreterQueryPlan.cpp QueryPlans/InterpreterQueryPlan.h QueryPlans/SQLCondition/AbstractSQLCondition.h QueryPlans/SQLCondition/BasicSQLCondition.h QueryPlans/SQLCondition/GreaterSQLCondition.h QueryPlans/SQLCondition/SQLConditionUtils.h QueryPlans/SQLCondition/SQLConditionFactory.h)

add_executable(db ${SOURCE_FILES})