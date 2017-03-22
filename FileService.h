//
// Created by 张程易 on 2017/3/20.
//

#ifndef DB_FILESERVICE_H
#define DB_FILESERVICE_H

#include "Common.h"

class FileService{
    std::vector<FILE *> files;
public:
    FileService();
    FileService(const FileService & ) = delete;
    FileService operator=(const FileService &) = delete;
    int openFile(const char * fileName);
    FILE * getFile(int fileID);
    size_t allocBlock(int fileID);
    void writeBlock(int fileID, size_t startOffset, void * data);
    size_t getBlockCnt(int fileID);
    void * readBlock(int fileID, size_t startOffset);
    ~FileService();
};

#endif //DB_FILESERVICE_H
