//
// Created by 张程易 on 12/06/2017.
//

#ifndef DB_INTERPRETERSERVICE_H
#define DB_INTERPRETERSERVICE_H


#include "RecordService.h"
#include "../SQLSession.h"

class InterpreterService {
    RecordService * recordService;
    MetaDataService * metaDataService;
    BlockService * blockService;

public:
    InterpreterService(RecordService *recordService, MetaDataService *metaDataService, BlockService *blockService);

    void start();

    void printResult(JSON * result);
};


#endif //DB_INTERPRETERSERVICE_H
