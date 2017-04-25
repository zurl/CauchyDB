//
// Created by 张程易 on 2017/3/31.
//

#include "TableModel.h"

TableModel::TableModel(FileService * fileService, BlockService * blockService, std::string && name, JSON * config, bool create)
        :name(name),blockService(blockService)
{
    // open file
    len = 0;
    this->fileService = fileService;
    if(create){
        fileService->createFile(name.c_str());
    }
    fid = fileService->openFile(name.c_str());
    assert(fid != -1);
    JSONArray * data = config->get("columns")->toArray();
    size_t on = 0;
    for(auto & column: data->getElements()){
        columns.emplace_back(column, on);
        on += (size_t)column->get("size")->toInteger()->value;
    }
    // build fast search keyindex
    for(int i = 0; i < columns.size(); i++){
        keyindex.insert(std::pair<std::string, int>(columns[i].getName(), i));
        lenTable.emplace(i, len);
        len+=columns[i].getSize();
    }
    JSONObject * jarr = config->get("indices")->toObject();
    for(auto & index : jarr->getHashMap()){
        int id = keyindex[index.second->get("on")->toJString()->str];
        indices.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(id),
                std::forward_as_tuple(fileService, blockService, name + "_" + index.first, index.first, index.second, lenTable[id], create, columns[id].getType(), columns[id].getSize())
        );
    }
}

IndexModel * TableModel::findIndexOn(int cid)  {
    auto iter = indices.find(cid);
    if(iter == indices.end())return nullptr;
    else return &iter->second;
}

JSON *TableModel::toJSON() {
    auto json = new JSONObject();
    auto jobj = new JSONObject();
    for(auto & x: indices){
        jobj->set(x.second.getName(),x.second.toJSON(this->columns[x.second.getOn()].getName()));
    }
    json->set("indices", jobj);
    auto jarr = new JSONArray();
    for(auto &x: columns){
        jarr->put(x.toJSON());
    }
    json->set("columns", jarr);
    return json;
}

int TableModel::getColumnIndex(const std::string & str) const {
    auto iter = keyindex.find(str);
    if( iter == keyindex.end()) throw SQLExecuteException(1, "unknown column");
    return iter->second;
}