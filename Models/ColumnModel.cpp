//
// Created by 张程易 on 2017/3/29.
//

#include "ColumnModel.h"


JSON * ColumnModel::toJSON(){
    auto json = new JSONObject();
    json->set("name", name);
    json->set("size", size);
    json->set("unique", unique);
    if( type == ColumnType::Int){
        json->set("type", "int");
    }
    else if( type == ColumnType::Float){
        json->set("type", "float");
    }
    else if( type == ColumnType::Char){
        json->set("type", "char");
    }
    return json;
}
