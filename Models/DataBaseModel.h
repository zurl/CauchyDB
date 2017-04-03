//
// Created by 张程易 on 2017/4/3.
//

#ifndef DB_DATABASEMODEL_H
#define DB_DATABASEMODEL_H

#include "../Common.h"
#include "../Services/FileService.h"
#include "TableModel.h"


class DataBaseModel{
    FileService * fileService;
    std::string name;
    std::map<std::string, TableModel * > tables;
public:
    DataBaseModel(FileService * fileService, const std::string & name, JSON * config);
    ~DataBaseModel();
    TableModel * getTableByName(const std::string & str);
    void createTable(const std::string & str, JSON * config);
};



#endif //DB_DATABASEMODEL_H
