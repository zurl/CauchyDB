//
// Created by 张程易 on 2017/4/3.
//

#ifndef DB_SQLSESSION_H
#define DB_SQLSESSION_H
#include "Common.h"
#include "Services/RecordService.h"
#include "Services/MetaDataService.h"

class SQLSession{
    MetaDataService * metaDataService;


private:
    DataBaseModel * dataBaseModel;
    RecordService * recordService;
    BlockService * blockService;
public:
    SQLSession(MetaDataService *metaDataService, DataBaseModel *dataBaseModel, RecordService *recordService,
               BlockService *blockService) ;

    RecordService *getRecordService() const ;
    BlockService *getBlockService() const ;
    bool loadDatabase(const std::string &name);
    TableModel * getTable(const std::string & name);
    DataBaseModel *getDataBaseModel() const;
    MetaDataService *getMetaDataService() const;
    void saveMetaData() const;
    inline bool hasDataBase(const std::string & name){ return metaDataService->hasDatabase(name);}
};


#endif //DB_SQLSESSION_H
