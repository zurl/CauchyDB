//
// Created by 张程易 on 2017/4/3.
//

#include "DataBaseModel.h"


DataBaseModel::DataBaseModel(FileService * fileService, BlockService * blockService, const std::string & name, JSON * config)
:fileService(fileService),blockService(blockService),name(name){
    if( config != nullptr){
        JSONObject * data = config->get("tables")->toObject();
        for(auto & table: data->getHashMap()){
            tables.emplace(table.first,
                           new TableModel(fileService, blockService, name + "_" + table.first + ".ct", table.second, false));
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
    tables.emplace(str,
                   new TableModel(fileService, blockService, name + "_" + str + ".ct", config, true));
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

bool DataBaseModel::hasTable(const std::string &str) {
    auto iter = tables.find(str);
    return iter != tables.end();
}

void DataBaseModel::dropTable(const std::string &str) {
    tables.erase(str);
}
