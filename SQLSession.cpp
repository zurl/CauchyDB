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

bool SQLSession::loadDatabase(const std::string &name){
    dataBaseModel = metaDataService->getDataBase(name);
    return dataBaseModel != nullptr;
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

QueryPlan *SQLSession::getLastQueryPlan() const {
    return lastQueryPlan;
}

void SQLSession::setLastQueryPlan(QueryPlan *lastQueryPlan) {
    SQLSession::lastQueryPlan = lastQueryPlan;
}

bool SQLSession::isFlush() const {
    return flush;
}

void SQLSession::setFlush(bool flush) {
    SQLSession::flush = flush;
}
