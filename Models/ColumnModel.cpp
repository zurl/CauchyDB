//
// Created by 张程易 on 2017/3/29.
//

#include "ColumnModel.h"


JSON * ColumnModel::toJSON(){
    auto json = new JSONObject();
    json->hashMap.emplace("name", new JSONString(name));
    json->hashMap.emplace("size", new JSONInteger(size));
    if( type == ColumnType::Int){
        json->hashMap.emplace("type", new JSONString("int"));
    }
    else if( type == ColumnType::Float){
        json->hashMap.emplace("type", new JSONString("float"));
    }
    else if( type == ColumnType::Char){
        json->hashMap.emplace("type", new JSONString("char"));
    }
    return json;
}