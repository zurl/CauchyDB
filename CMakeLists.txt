cmake_minimum_required(VERSION 3.6)
project(db)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES db.cpp JSON.cpp JSON.h JSONParser.cpp BPlusTree.h Common.h BlockService.cpp BlockService.h FileService.cpp FileService.h SQLParser.cpp SQLParser.h Configuration.cpp Configuration.h RecordService.cpp RecordService.h QueryFilter.h QueryScanner.cpp QueryScanner.h LinearQueryScanner.cpp LinearQueryScanner.h RangeIndexQueryScanner.h OneIndexQueryScanner.h RangeIndexQueryScannerFactory.h ColumnModel.cpp ColumnModel.h OneIndexQueryScannerFactory.h TableModel.cpp TableModel.h IndexModel.cpp IndexModel.h)

add_executable(db ${SOURCE_FILES})