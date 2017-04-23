//
// Created by 张程易 on 2017/4/3.
//

#include "SQLSession.h"



SQLSession::SQLSession(MetaDataService *metaDataService, DataBaseModel *dataBaseModel, RecordService *recordService,
           BlockService *blockService) : metaDataService(metaDataService), dataBaseModel(dataBaseModel),
                                         recordService(recordService), blockService(blockService) {}

RecordService *SQLSession::getRecordService() const {
    return recordService;
}

BlockService *SQLSession::getBlockService() const {
    return blockService;
}

void SQLSession::loadTable(const std::string & name){
    dataBaseModel = metaDataService->getDataBase(name);
}

TableModel * SQLSession::getTable(const std::string & name){
    if( dataBaseModel == nullptr) return nullptr;
    return dataBaseModel->getTableByName(name);
}

DataBaseModel *SQLSession::getDataBaseModel() const {
    return dataBaseModel;
}

MetaDataService *SQLSession::getMetaDataService() const {
    return metaDataService;
}

void SQLSession::saveMetaData() const {
    metaDataService->saveIntoFile();
}
