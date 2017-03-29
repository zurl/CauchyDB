//
// Created by 张程易 on 2017/3/29.
//

#include "ColumnModel.h"

ColumnModel::ColumnModel( JSON * config ){
    JSONObject * data = config->toObject();
    name = data->get("name")->asCString();
    size = (size_t)data->get("typeSize")->toInteger()->value;
    const char * typestr = data->get("type")->asCString();
    if( strcmp(typestr, "int") == 0){
        type = Type::Int;
    }
    else if( strcmp(typestr, "char") == 0){
        type = Type::Char;
    }
    else if( strcmp(typestr, "float") == 0){
        type = Type::Float;
    }
    else throw "fuck type";
}

size_t ColumnModel::getSize() const {
    return size;
}

const std::string &ColumnModel::getName() const {
    return name;
}
