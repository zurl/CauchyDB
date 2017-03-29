//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_COLUMNMODEL_H
#define DB_COLUMNMODEL_H

#include "Common.h"

class ColumnModel{
    std::string name;
    size_t size;
    Type type;
public:
    enum class Type{
        Char, Int, Float
    };
    ColumnModel( JSON * config ){
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

    size_t getSize() const {
        return size;
    }

    Type getType() const {
        return type;
    }


    const std::string &getName() const {
        return name;
    }
};

#endif //DB_COLUMNMODEL_H
