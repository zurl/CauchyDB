//
// Created by 张程易 on 2017/3/31.
//

#ifndef DB_INDEXMODEL_H
#define DB_INDEXMODEL_H

#include "../Services/FileService.h"
#include "../AbstractIndexRunner.h"
#include "../IndexRunner.h"


class IndexModel{
    int fid;
    size_t on;
    FileService * fileService;
    AbstractIndexRunner * indexRunner;
    std::string name;
    ColumnType columnType;
    int columnSize;
    public:
    IndexModel(FileService * fileService, BlockService * blockService,
                      const std::string & fname, const std::string & name,
                      JSON * config, size_t on, bool create, ColumnType columnType, int columnSize);

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
