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
    for(auto & db: dbs->getHashMap()){
        dataBases.emplace(db.first, new DataBaseModel(fileService, db.first, db.second));
    }
}

void MetaDataService::createDataBase(const std::string & name){
    auto iter = dataBases.find(name);
    if(iter != dataBases.end()) throw SQLExecuteException(10, "used name");
    dataBases.emplace(name, new DataBaseModel(fileService, name, nullptr));
}

bool MetaDataService::hasDatabase(const std::string &name){
    auto iter = dataBases.find(name);
    return iter != dataBases.end();
}

JSON * MetaDataService::toJSON() {
    auto json = new JSONObject();
    auto jobj = new JSONObject();
    for(auto &x : dataBases){
        jobj->set(x.first, x.second->toJSON());
    }
    json->set("databases", jobj);
    return json;
}

void MetaDataService::saveIntoFile() {
    JSON * json = this->toJSON();
    const char * fname = Configuration::attrCString("meta_file_path");
    FILE * fp = fopen(fname, "w");
    fputs(json->toString(true).c_str(), fp);
    fclose(fp);
}
