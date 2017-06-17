//
// Created by 张程易 on 12/06/2017.
//

#include "InterpreterService.h"
#include "../SQLParser.h"

InterpreterService::InterpreterService(RecordService *recordService, MetaDataService *metaDataService,
                                       BlockService *blockService) : recordService(recordService),
                                                                     metaDataService(metaDataService),
                                                                     blockService(blockService) {}

void InterpreterService::start() {
    printf("Welcome to the CauchyDB REPL, Commands end with ';'.\n");
    printf("Server version: 1.0.0. CauchyDB Open Source Version (MIT)\n");
    session = new SQLSession(metaDataService, nullptr, recordService, blockService);
    parser = new SQLParser(session);
    char buffer[1024];  int c;
    while(true){
        char *ptr = buffer;
        std::cout<<">>>";
        fflush(stdout);
        while((c=getchar())!=';'){
            *ptr++ = (char)c;
        }
        *ptr++ = ';';
        *ptr = 0;
        if(strncmp(buffer, "quit", 4) == 0||strncmp(buffer, "\nquit", 5) == 0) break;
        if(strncmp(buffer, "execfile", 8) == 0||strncmp(buffer, "\nexecfile", 9) == 0){
            // execute file
            char * startPtr = buffer[7] == 'e' ? buffer + 9 : buffer + 10;
            int len = 0;
            while(*(startPtr + len) != ';')len++;
            *(startPtr + len) = 0;
            FILE * file = fopen(startPtr, "r");
            if(file == nullptr){
                std::cout<<"[ERROR:10] File Not Found"<<std::endl;
                continue;
            }
            ptr = buffer;
            while((c=fgetc(file)) != EOF){
                *ptr++ = (char)c;
                if(c == ';'){
                    *ptr = 0;
                    execute(buffer);
                    ptr = buffer;
                }
            }
            if(session->isFlush()) blockService->synchronize();
            continue;
        }
        execute(buffer);
        if(session->isFlush()) blockService->synchronize();
    }
}

void InterpreterService::printResult(JSON *result) {
    int code = (int)result->get("status")->toInteger()->value;
    if(code == 0){
        std::cout<<"Query OK, "<<result->get("message")->toJString()->str<<std::endl;
    }
    else if(code == 1){
        std::cout<<"Query OK, "<<result->get("data")->toInteger()->value<<" row affected."<<std::endl;
    }
    else if(code == 2){
        int sz = 0;
        bool flag = false;
        auto & elems = result->get("data")->toArray()->getElements();
        if(elems.size() > 0){
            sz = (int)elems[0]->toArray()->getElements().size();
        }
        std::cout<<"|";
        for(int i = 0; i < sz; i++){
            std::cout<<"----------|";
        }
        std::cout<<std::endl;
        for(auto & row: elems){
            std::cout<<"|";
            for(auto & item: row->toArray()->getElements()){
                std::cout.width(10);
                std::cout<<item->toString();
                std::cout<<"|";
            }
            std::cout<<std::endl;
            if(!flag){
                flag = true;
                std::cout<<"|";
                for(int i = 0; i < sz; i++){
                    std::cout<<"----------|";
                }
                std::cout<<std::endl;
            }
        }
        std::cout<<"|";
        for(int i = 0; i < sz; i++){
            std::cout<<"----------|";
        }
        std::cout<<std::endl;
        std::cout<<"Query OK, "<<elems.size() - 1<<" row selected."<<std::endl;
    }
    else{
        std::cout<<"[ERROR:"<<code<<"] : "<<result->get("message")->toJString()->str<<std::endl;
    }
}

void InterpreterService::execute(char *buffer) {
    try{
        QueryPlan * sql = parser->parseSQLStatement(buffer);
        JSON * result = sql->runQuery(recordService);
        if( result == nullptr ) {
            std::cout<<"[ERROR:0] : Invalid SQL Command"<<std::endl;
        }
        printResult(result);
        session->setLastQueryPlan(sql);
    }catch(SQLException & e){
        std::cout<<"[ERROR:"<<e.code<<"] : "<<e.message<<std::endl;
    }
}
