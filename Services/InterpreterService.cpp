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
    SQLSession * session = new SQLSession(metaDataService, nullptr, recordService, blockService);
    SQLParser * parser = new SQLParser(session);
    char buffer[1024];  int c;
    while(true){
        char *ptr = buffer;
        std::cout<<">>>";
        fflush(stdout);
        while((c=getchar())!=';'){
            *ptr++ = (char)c;
        }
        *ptr++ = ';';
        *ptr++ = 0;
        if(strncmp(buffer, "quit", 4) == 0||strncmp(buffer, "\nquit", 5) == 0) break;
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
        for(auto & row: result->get("data")->toArray()->getElements()){
            std::cout<<"|";
            for(auto & item: row->toArray()->getElements()){
                std::cout.width(10);
                std::cout<<item->toString();
                std::cout<<"|";
                sz++;
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
    }
    else{
        std::cout<<"[ERROR:"<<code<<"] : "<<result->get("message")->toJString()->str<<std::endl;
    }
}
