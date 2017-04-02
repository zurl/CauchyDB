//
// Created by 张程易 on 2017/3/31.
//

#ifndef DB_INDEXMODEL_H
#define DB_INDEXMODEL_H

#include "../Services/FileService.h"
#include "../AbstractIndexRunner.h"


class IndexModel{
    int fid;
    FileService * fileService;
    AbstractIndexRunner * indexRunner;
    public:
    inline IndexModel(FileService * fileService, std::string && name, JSON * config){
        this->fileService = fileService;
        fid = fileService->openFile((name + ".cdi").c_str());
    }

    inline int getFid() const {
        return fid;
    }

    AbstractIndexRunner *getIndexRunner() {
        return indexRunner;
    }
};

#endif //DB_INDEXMODEL_H
