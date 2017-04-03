//
// Created by 张程易 on 2017/4/3.
//

#include "MetaDataService.h"



MetaDataService::~MetaDataService(){
    for( auto & x: dataBases){
        delete x.second;
    }
}
DataBaseModel * MetaDataService::getDataBase(const std::string & name){
    return dataBases[name];
}
MetaDataService::MetaDataService(FileService * fileService)
        :fileService(fileService){
    data = JSON::fromFile(Configuration::attrCString("meta_file_path"));
    // Load All DataBases
    JSONObject * dbs = data->get("databases")->toObject();
    for(auto & db: dbs->hashMap){
        dataBases.emplace(db.first, new DataBaseModel(fileService, db.first, db.second));
    }
}

void MetaDataService::createDataBase(const std::string & name){
    auto iter = dataBases.find(name);
    if(iter != dataBases.end()) throw SQLExecuteException(10, "used name");
    dataBases.emplace(name, new DataBaseModel(fileService, name, nullptr));
}

JSON *MetaDataService::toJSON() {
    auto json = new JSONObject();
    auto jobj = new JSONObject();
    for(auto &x : dataBases){
        jobj->hashMap.emplace(x.first, x.second->toJSON());
    }
    json->hashMap.emplace("databases", jobj);
    return json;
}
