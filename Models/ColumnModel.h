//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_COLUMNMODEL_H
#define DB_COLUMNMODEL_H

#include "../Common.h"

class ColumnModel{

public:
    enum class Type{
        Char, Int, Float
    };
    inline ColumnModel( JSON * config ){
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

    inline size_t getSize() const {
        return size;
    }

    inline Type getType() const {
        return type;
    }

    inline const std::string &getName() const {
        return name;
    }

private:
    std::string name;
    size_t size;
    Type type;
};

#endif //DB_COLUMNMODEL_H