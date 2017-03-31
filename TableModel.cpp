//
// Created by 张程易 on 2017/3/31.
//

#include "TableModel.h"

TableModel::TableModel(FileService * fileService, std::string && name, JSON * config)
        :name(name)
{
    // open file
    this->fileService = fileService;
    fid = fileService->openFile((name + ".cdt").c_str());
    JSONArray * data = config->get("columns")->toArray();
    for(auto & column: data->elements){
        columns.emplace_back(column);
    }
    // build fast search keyindex
    for(int i = 0; i < columns.size(); i++){
        keyindex.emplace(columns[i].getName(), i);
    }
    JSONObject * jarr = config->get("indices")->toObject();
    for(auto & index : jarr->hashMap){
        indices.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(keyindex[index.second->get("on")->asCString()]),
                std::forward_as_tuple(fileService, name + "_" + index.first, index.second)
        );
    }
}

IndexModel * TableModel::findIndexOn(int cid)  {
    auto iter = indices.find(cid);
    if(iter == indices.end())return nullptr;
    else return &iter->second;
}
