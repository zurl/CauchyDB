//
// Created by 张程易 on 2017/3/31.
//

#ifndef DB_INDEXMODEL_H
#define DB_INDEXMODEL_H

#include "../Services/FileService.h"
#include "../AbstractIndexRunner.h"


class IndexModel{
    int fid;
    size_t on;
    FileService * fileService;
    AbstractIndexRunner * indexRunner;
    std::string name;
    public:
    inline IndexModel(FileService * fileService, const std::string & fname, const std::string & name, JSON * config, size_t on):name(name){
        this->fileService = fileService;
        fid = fileService->openFile((fname + ".cdi").c_str());
        this->on = on;
    }

    inline int getFid() const {
        return fid;
    }

    inline AbstractIndexRunner *getIndexRunner()const {
        return indexRunner;
    }

    const std::string &getName() const {
        return name;
    }

    inline size_t getOn() const {
        return on;
    }

    JSON * toJSON(const std::string & on);
};

#endif //DB_INDEXMODEL_H
