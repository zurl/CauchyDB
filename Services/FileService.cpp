//
// Created by 张程易 on 2017/3/20.
//

#include "FileService.h"

char zeroBuffer[BLOCK_SIZE * 2];


FileService::FileService(){}
int FileService::openFile(const char * fileName){
    FILE * fp = fopen(fileName,"rb+");
    if( fp == NULL ) return -1;
    files.emplace_back(fp);
    fseek(fp, 0, SEEK_END);
    int fid = (int)(files.size()) - 1;
    fileBlockCnt.push_back((size_t)ftell(fp) / BLOCK_SIZE);
    return fid;
}
FILE * FileService::getFile(int fileID){
    assert(fileID < files.size() && fileID >= 0);
    return files[fileID];
}
size_t FileService::allocBlock(int fileID){
    assert(fileID < files.size() && fileID >= 0);
    FILE * fp = files[fileID];
    fseek(fp, 0, SEEK_END);
    size_t offset = (size_t)ftell(fp);
    fwrite(zeroBuffer, BLOCK_SIZE, 1,  fp);
    fflush(fp);
    fileBlockCnt[fileID] ++;
    return offset / BLOCK_SIZE;
}
void FileService::writeBlock(int fileID, size_t startOffset, void * data){
    assert(fileID < files.size() && fileID >= 0);
    FILE * fp = files[fileID];
    int a = fseek(fp, startOffset * BLOCK_SIZE, SEEK_SET);
    fwrite(data, BLOCK_SIZE, 1, fp);
    fflush(fp);
}
size_t FileService::getBlockCnt(int fileID){
    assert(fileID < files.size());
    return fileBlockCnt[fileID];
}
void * FileService::readBlock(int fileID, size_t startOffset){
    assert(fileID < files.size());
    FILE * fp = files[fileID];
    fseek(fp, startOffset * BLOCK_SIZE, SEEK_SET);
    char * result = new char[BLOCK_SIZE];
    fread(result, BLOCK_SIZE, 1, fp);
    return result;
}
FileService::~FileService(){
    for(auto & fp : files){
        fclose(fp);
    }
}
