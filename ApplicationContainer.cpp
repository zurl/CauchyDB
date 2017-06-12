//
// Created by 张程易 on 12/06/2017.
//

#include "ApplicationContainer.h"

ApplicationContainer::ApplicationContainer() {
    Configuration::initialize();
    fileService = new FileService();
    blockService = new BlockService(fileService);
    recordService = new RecordService(blockService);
    metaDataService = new MetaDataService(fileService, blockService);
    interpreterService = new InterpreterService(recordService, metaDataService, blockService);
}

ApplicationContainer::~ApplicationContainer() {
    if(blockService != nullptr)delete blockService;
    if(fileService != nullptr)delete fileService;
    if(metaDataService != nullptr)delete metaDataService;
    if(recordService != nullptr)delete recordService;
    if(interpreterService != nullptr)delete interpreterService;
}

InterpreterService *ApplicationContainer::getInterpreterService() const {
    return interpreterService;
}

