//
// Created by 张程易 on 2017/4/3.
//

#include "DataBaseModel.h"


DataBaseModel::DataBaseModel(FileService * fileService, const std::string & name, JSON * config)
:fileService(fileService),name(name){
    if( config != nullptr){
        JSONObject * data = config->get("tables")->toObject();
        for(auto & table: data->getHashMap()){
            tables.emplace(table.first,
                           new TableModel(fileService, name + "_" + table.first + ".ct", table.second));
        }
    }
}
DataBaseModel::~DataBaseModel(){
    for( auto & x : tables){
        delete x.second;
    }
}
TableModel * DataBaseModel::getTableByName(const std::string & str){
    return tables[str];
}

void DataBaseModel::createTable(const std::string &str, JSON *config) {
    fileService->createFile((name + "_" + str + ".ct").c_str());
    tables.emplace(str,
                   new TableModel(fileService, name + "_" + str + ".ct", config));
}

JSON *DataBaseModel::toJSON() {
    auto json = new JSONObject();
    auto jobj = new JSONObject();
    for(auto &x : tables){
        jobj->set(x.first, x.second->toJSON());
    }
    json->set("tables", jobj);
    return json;
}
