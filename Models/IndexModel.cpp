//
// Created by 张程易 on 2017/3/31.
//

#include "IndexModel.h"

JSON * IndexModel::toJSON(const std::string & on) {
    auto json = new JSONObject();
    json->set("type", "bplus");
    json->set("size", columnSize);
    json->set("on", on);
    return json;
}

IndexModel::IndexModel(FileService * fileService, BlockService * blockService, const std::string & fname, const std::string & name,
        JSON * config, int on, bool create, ColumnType columnType, int columnSize):name(name){
        this->columnType = columnType;
        this->fileService = fileService;
        if(create){
            fileService->createFile((fname + ".cdi").c_str());
            fid = fileService->openFile((fname + ".cdi").c_str());
            fileService->allocBlock(fid);
        }else{
            fid = fileService->openFile((fname + ".cdi").c_str());
        }
        if(columnType == ColumnType::Char){
            int size = columnSize;
            if(size <= 16){
                indexRunner = new IndexRunner<char[16]>(fid, blockService);
            }
            else if(size <= 64){
                indexRunner = new IndexRunner<char[64]>(fid, blockService);
            }
            else{
                indexRunner = new IndexRunner<char[256]>(fid, blockService);
            }
        }
        else if(columnType == ColumnType::Int){
            indexRunner = new IndexRunner<int>(fid, blockService);
        }
        else{
            indexRunner = new IndexRunner<double>(fid, blockService);
        }
        if(create){
            indexRunner->initialize();
        }
        this->on = on;
}