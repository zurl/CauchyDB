//
// Created by 张程易 on 2017/3/22.
//

#include "Configuration.h"

void Configuration::initialize(){
    const char * CONFIG_FILE_NAME = "config.json";
    config = JSON::fromFile(CONFIG_FILE_NAME);
}
const char * Configuration::attrCString(const char * key){
    return config->get(key)->asCString();
}
JSON * Configuration::getJSON(){
    return config;
}
JSON * Configuration::config = nullptr;

