//
// Created by 张程易 on 2017/3/31.
//

#include "IndexModel.h"

JSON * IndexModel::toJSON(const std::string & on) {
    auto json = new JSONObject();
    json->set("type", "bplus");
    json->set("on", on);
    return json;
}