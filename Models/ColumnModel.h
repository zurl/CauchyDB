//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_COLUMNMODEL_H
#define DB_COLUMNMODEL_H

#include "../Common.h"

class ColumnModel{

public:
    inline ColumnModel( JSON * config, int on ){
        JSONObject * data = config->toObject();
        name = data->get("name")->toJString()->str.c_str();
        size = (int)data->get("size")->toInteger()->value;
        this->on = on;
        const char * typestr = data->get("type")->toJString()->str.c_str();
        if( strcmp(typestr, "int") == 0){
            type = ColumnType::Int;
        }
        else if( strcmp(typestr, "char") == 0){
            type = ColumnType::Char;
        }
        else if( strcmp(typestr, "float") == 0){
            type = ColumnType::Float;
        }
        else throw "fuck type";
    }

    inline int getSize() const {
        return size;
    }

    inline ColumnType getType() const {
        return type;
    }

    inline const std::string &getName() const {
        return name;
    }

    inline int getOn() const {
        return on;
    }

    JSON * toJSON();
private:
    std::string name;
    int size;
    ColumnType type;
    int on;
};

#endif //DB_COLUMNMODEL_H
