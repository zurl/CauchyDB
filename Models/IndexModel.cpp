//
// Created by 张程易 on 2017/3/31.
//

#include "IndexModel.h"

JSON * IndexModel::toJSON() {
    auto json = new JSONObject();
    json->hashMap.emplace("type", new JSONString("bplus"));
    json->hashMap.emplace("name", new JSONString(name));
    return json;
}