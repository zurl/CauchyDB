cmake_minimum_required(VERSION 3.6)
project(db)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES db.cpp JSON/JSON.cpp JSON/JSON.h JSON/JSONParser.cpp BPlusTree.h Common.h Services/BlockService.cpp Services/BlockService.h Services/FileService.cpp Services/FileService.h SQLParser.cpp SQLParser.h Configuration.cpp Configuration.h Services/RecordService.cpp Services/RecordService.h QueryFilter.h Scanners/QueryScanner.cpp Scanners/QueryScanner.h Scanners/LinearQueryScanner.cpp Scanners/LinearQueryScanner.h Scanners/RangeIndexQueryScanner.h Scanners/OneIndexQueryScanner.h Scanners/RangeIndexQueryScannerFactory.h Models/ColumnModel.cpp Models/ColumnModel.h Scanners/OneIndexQueryScannerFactory.h Models/TableModel.cpp Models/TableModel.h Models/IndexModel.cpp Models/IndexModel.h)

add_executable(db ${SOURCE_FILES})