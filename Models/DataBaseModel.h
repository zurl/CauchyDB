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
    BlockService * blockService;
    std::string name;
    std::map<std::string, TableModel * > tables;
public:
    DataBaseModel(FileService * fileService, BlockService * blockService, const std::string & name, JSON * config);
    ~DataBaseModel();
    TableModel * getTableByName(const std::string & str);
    void createTable(const std::string & str, JSON * config);
    inline bool hasTable(const std::string & str){
        auto iter = tables.find(str);
        return iter != tables.end();
    }
    inline JSON * getTables(){
        JSONArray * jarr = new JSONArray();
        for(auto & pair : tables){
            jarr->put(new JSONString(pair.first));
        }
        return jarr;
    }
    JSON * toJSON();
};



#endif //DB_DATABASEMODEL_H
