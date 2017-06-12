cmake_minimum_required(VERSION 3.6)
project(db)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall")

set(SOURCE_FILES
        Main.cpp
        JSON/JSON.cpp
        JSON/JSON.h
        JSON/JSONParser.cpp
        Index/BPlusTree.h
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
        Index/AbstractIndexRunner.h
        Index/IndexRunner.h
        Scanners/RangeIndexQueryScanner.cpp
        Scanners/OneIndexQueryScanner.cpp
        Scanners/LinearQueryScanner.cpp
        SQLSession.cpp SQLSession.h
        Services/MetaDataService.cpp
        Services/MetaDataService.h
        Models/DataBaseModel.cpp
        Models/DataBaseModel.h
        Network/TCP/TCPServer.cpp
        Network/HTTP/HTTPMethod.h
        Network/HTTP/HTTPServer.h
        Network/HTTP/HTTPRequest.h
        Network/HTTP/HTTPResponse.h
        Network/HTTP/HTTPRequestHeader.h
        Network/HTTP/HTTPResponseHeader.h
        Network/HTTP/HTTPServer.h
        Network/HTTP/HTTPContext.h
        Network/HTTP/HTTPHeader.h
        QueryPlans/QueryPlan.h
        QueryPlans/InsertQueryPlan.cpp
        QueryPlans/InsertQueryPlan.h
        QueryPlans/SelectQueryPlan.cpp
        QueryPlans/SelectQueryPlan.h
        QueryPlans/SQLWhereClause.cpp
        QueryPlans/SQLWhereClause.h
        QueryPlans/DDL/CreateQueryPlan.h
        QueryPlans/DDL/CreateTableQueryPlan.cpp
        QueryPlans/DDL/CreateTableQueryPlan.h
        QueryPlans/DDL/CreateDataBaseQueryPlan.cpp
        QueryPlans/DDL/CreateDataBaseQueryPlan.h
        QueryPlans/InterpreterQueryPlan.cpp
        QueryPlans/InterpreterQueryPlan.h
        QueryPlans/SQLCondition/EqualSQLCondition.h
        QueryPlans/SQLCondition/GreaterOrEqualSQLCondition.h
        QueryPlans/SQLCondition/LessOrEqualSQLCondition.h
        QueryPlans/SQLCondition/LessSQLCondition.h
        QueryPlans/SQLCondition/NotEqualSQLCondition.h
        QueryPlans/SQLCondition/AbstractSQLCondition.h
        QueryPlans/SQLCondition/BasicSQLCondition.h
        QueryPlans/SQLCondition/GreaterSQLCondition.h
        QueryPlans/SQLCondition/SQLConditionUtils.h
        QueryPlans/SQLCondition/SQLConditionFactory.h
        QueryPlans/DDL/DropDataBaseQueryPlan.cpp
        QueryPlans/DDL/DropDataBaseQueryPlan.h
        QueryPlans/DDL/DropTableQueryPlan.cpp
        QueryPlans/DDL/DropTableQueryPlan.h
        QueryPlans/DDL/DropQueryPlan.h
        Exception.h
        QueryPlans/DDL/DropIndexQueryPlan.cpp
        QueryPlans/DDL/DropIndexQueryPlan.h
        QueryPlans/DDL/CreateIndexQueryPlan.cpp
        QueryPlans/DDL/CreateIndexQueryPlan.h QueryPlans/DeleteQueryPlan.cpp QueryPlans/DeleteQueryPlan.h Services/InterpreterService.cpp Services/InterpreterService.h ApplicationContainer.cpp ApplicationContainer.h)

add_executable(db ${SOURCE_FILES})