//
// Created by 张程易 on 12/06/2017.
//

#ifndef DB_APPLICATIONCONTAINER_H
#define DB_APPLICATIONCONTAINER_H


#include "Services/BlockService.h"
#include "Services/FileService.h"
#include "Services/MetaDataService.h"
#include "Services/RecordService.h"
#include "Services/InterpreterService.h"

class ApplicationContainer {
    BlockService * blockService = nullptr;
    FileService * fileService = nullptr;
    MetaDataService * metaDataService = nullptr;
    RecordService * recordService = nullptr;
    InterpreterService * interpreterService = nullptr;
public:
    ApplicationContainer();

    InterpreterService *getInterpreterService() const;

    virtual ~ApplicationContainer();
};


#endif //DB_APPLICATIONCONTAINER_H
