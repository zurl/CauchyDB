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
    void dropTable(const std::string & str);
    bool hasTable(const std::string & str);
    inline JSON * getTables(){
        JSONArray * jarr = new JSONArray();
        JSONArray * title = new JSONArray();
        title->put(new JSONString("Tables"));
        jarr->put(title);
        for(auto & pair : tables){
            JSONArray * elem = new JSONArray();
            elem->put(new JSONString(pair.first));
            jarr->put(elem);
        }
        return jarr;
    }
    JSON * toJSON();
};



#endif //DB_DATABASEMODEL_H
