cmake_minimum_required(VERSION 3.6)
project(db)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES db.cpp json.cpp json.h jsonParser.cpp BPlusTree.h Common.h BlockService.cpp BlockService.h FileService.cpp FileService.h SQLParser.cpp SQLParser.h Configuration.cpp Configuration.h RecordService.cpp RecordService.h)

add_executable(db ${SOURCE_FILES})