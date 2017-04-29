
#include "Common.h"
#include "BPlusTree.h"
#include "Configuration.h"
#include "QueryFilter.h"
#include "SQLSession.h"
#include "Models/ColumnModel.h"
#include "Models/IndexModel.h"
#include "Models/TableModel.h"
#include "Models/DataBaseModel.h"
#include "Services/FileService.h"
#include "Services/BlockService.h"
#include "Services/MetaDataService.h"
#include "Services/RecordService.h"
#include "SQLParser.h"
using namespace std;



/*
 * a>=35 && a<=35
 * SQL => QueryPlan
 * QueryPlan
 * type => select
 * front => linear / index
 *      index: { on : "primiary", "
 *
 *
 */

#include<sys/time.h>
long long getTime(){
    struct  timeval    tv;
    struct  timezone   tz;
    gettimeofday(&tv,&tz);
    return tv.tv_usec + tv.tv_sec * 1000000;
}

class ApplicationContainer{
public:
    BlockService * blockService;
    FileService * fileService;
    MetaDataService * metaDataService;
    RecordService * recordService;
    ApplicationContainer(){}
    ~ApplicationContainer(){
        delete blockService;
        delete fileService;
        delete metaDataService;
        delete recordService;
    }
    void start(){
        // The bootstrap persedure of IoC Container:
        // Initial Configuration Singleton
        // Load FileService
        // Load MetaDataService { Register File => FileService }
        // Load BlockService { Inject FileService }
        // Load QueryRunnerService{ Inject BlockService }
        // Load QueryBuilderService { Inject QueryRunnerService }
        // Load SQLInterpreterService { Inject QueryBuilderSerice }
        Configuration::initialize();
        fileService = new FileService();
        blockService = new BlockService(fileService);
        recordService = new RecordService(blockService);
        metaDataService = new MetaDataService(fileService, blockService);
    }
    void testSQLBatch(){
        auto session = new SQLSession(metaDataService, nullptr, recordService, blockService);
        auto parser = new SQLParser(session);
        char x[256];
        //session->loadDatabase("fuck");
        while(true){
            cin.getline(x, 210);
            if(x[0] == '@')break;
            if(x[0] == '#'){
                long long f = getTime();
                //session->loadDatabase("madan");
                for(int i = 0; i <= 100000; i++){
                    auto str = std::string("insert into mdzz (id, value) values (")
                    + std::string(itos(i)) + ",1.1);";
                    auto sql = parser->parseSQLStatement(str.c_str());
                    std::string();
                    sql->runQuery(recordService);
                }
                long long e = getTime();
                std::cout<<"In : "<<e-f<<" ns."<<std::endl;
            }
            try{
                long long f = getTime();
                auto sql = parser->parseSQLStatement(x);
                if(!sql){
                    std::cout <<"[ERROR] Invalid SQL Command"<<std::endl;
                    continue;
                }
                std::cout<<sql->toJSON()->toString(true)<<endl;
                auto result = sql->runQuery(recordService);
                long long e = getTime();
                if(!result){
                    std::cout <<"[ERROR] Invalid SQL Command"<<std::endl;
                    continue;
                }
                std::cout<<result->toString(true)<<endl;
                std::cout<<"In : "<<e-f<<" ns."<<std::endl;
            }catch(SQLException & e){
                std::cout<<"[ERROR]("<<e.code<<") : "<<e.message<<std::endl;
            }
        }
    }

    void testSQL(){
        auto session = new SQLSession(metaDataService, nullptr, recordService, blockService);
        session->loadDatabase("test");
        auto parser = new SQLParser(session);
        parser->tokenize();
        parser->test();
        //auto sql = parser->parseSQLStatement();
//        std::cout<<sql->toJSON()->toString(true)<<endl;
//        std::cout<<sql->runQuery(recordService)->toString(true)<<endl;
//        std::cout<<metaDataService->toJSON()->toString(true)<<endl;
    }

#define __fo(x) ((x) / BLOCK_SIZE )
#define __bo(x) ((x) % BLOCK_SIZE )

    void testRecord(int fid){
        char buffer[BLOCK_SIZE];
        buffer[0] = 'a';
        buffer[1] = 'v';
        buffer[2] = 0;
        auto a = recordService->insert(fid, buffer, 8);
        buffer[1] = '1';
        auto b = recordService->insert(fid, buffer, 8);
        buffer[1] = '2';
        auto c = recordService->insert(fid, buffer, 8);
        buffer[1] = '3';
        auto d = recordService->insert(fid, buffer, 8);
        printf("read from record 0 %s\n", recordService->read(fid, __fo(a),__bo(a), 8));
        printf("read from record 1 %s\n", recordService->read(fid, __fo(b),__bo(b), 8));
        printf("read from record 2 %s\n", recordService->read(fid, __fo(c),__bo(c), 8));
        printf("read from record 3 %s\n", recordService->read(fid, __fo(d),__bo(d), 8));
        recordService->write(fid, 0, 8, buffer, 8);
        //recordFacade->remove(fid, 16, 8);
        for(int i = 0; i<= 50; i++){
            printf("read from record %d %s\n",
                   i,
                   recordService->read(fid,__fo(i * 8), __bo(i * 8), 8));
        }
    }
    void testFile(){
        int fid = fileService->openFile("aes.jb");
        printf("open file: fid = %d\n", fid);
        int offset = fileService->allocBlock(fid);
        printf("alloc block at offset = %d\n", (int) offset);
        char * data = new char[BLOCK_SIZE];
        data[0] = 'a';
        data[1] = 'b';
        data[2] = 0;
        fileService->writeBlock(fid, offset, (void *)data);
        printf("write block with str = %s\n", data);
        char * out = (char *)fileService->readBlock(fid, offset);
        printf("read block with str = %s\n", out);
        delete[] out;
        delete[] data;
    }
    void testBlockSr(int fid){
        int offset = blockService->allocBlock(fid);
        BlockItem * blk = blockService->getBlock(fid, offset);
        char * data = (char * )blk->value;
        data[0] = 'a';
        data[1] = 'b';
        blk->modified = 1;
        blk = blockService->getBlock(fid, offset);
        blk = blockService->getBlock(fid, offset);
        blk = blockService->getBlock(fid, offset);
        blk = blockService->getBlock(fid, offset);
        blk = blockService->getBlock(fid, offset);

    }
    void testBlock(){
        int fid = fileService->openFile("aes.jb");
        for(int i=1;i<=20; i++)testBlockSr(fid);
    }

    void testQueryScanner(int fid){
        char buffer[BLOCK_SIZE];
        int len = 8;
        buffer[0] = 'a';
        buffer[1] = 'v';
        buffer[2] = 0;
        auto a = recordService->insert(fid, buffer, len);
        buffer[1] = '1';
        auto b = recordService->insert(fid, buffer, len);
        buffer[1] = '2';
        auto c = recordService->insert(fid, buffer, len);
        buffer[1] = '3';
        auto d = recordService->insert(fid, buffer, len);
        printf("read from record 0 %s\n", recordService->read(fid, __fo(a),__bo(a), 8));
        printf("read from record 1 %s\n", recordService->read(fid, __fo(b),__bo(b), 8));
        printf("read from record 2 %s\n", recordService->read(fid, __fo(c),__bo(c), 8));
        printf("read from record 3 %s\n", recordService->read(fid, __fo(d),__bo(d), 8));
        LinearQueryScanner * linearQueryScanner = new LinearQueryScanner(recordService, fid,len);
        linearQueryScanner->scan([](int id, void * ptr){
            printf("scan %lld => %s\n", (long long)id, ptr);
        });
    }
};

//const int NODE_SIZE = 5;
//const int NODE_SIZE_HALF = (NODE_SIZE + 1) / 2; // 2
// 4 bytes (int) 4 bytes(float) 4 bytes char???fucking fuck
// NODE_SIZE_EQU ::=
// NODE_SIZE = (BLOCK_SIZE - 8 ) / ( key_type_size );
// 2 bytes size + 2 bytes isLeaf
// 8 bytes int, 8 bytes long, 16 bytes char, 64 bytes, 256 bytes
#include <sstream>
const char * itos(int i){
    char * buffer = new char[36];
    ostringstream os;
    os << i;
    strcpy(buffer, os.str().c_str());
    return buffer;
}





//BPlusTree<char[16]> bPlusTree;
//int main(){
//    bPlusTree.T_BPLUS_TEST();
//    return 0;
//}
//
//
//int main2(){
//    tokenize();
//    for(auto & x : tokens){
//        for(int i = x.begin; i <= x.end; i++)putchar(str[i]);
//        printf("\n");
//    }
//    return 0;
//}

int main(){
    ApplicationContainer applicationContainer;
    applicationContainer.start();
    applicationContainer.testSQLBatch();
    //int fid = applicationContainer.fileService->openFile("test.idx");
//    BPlusTree<char[16]> bPlusTree(applicationContainer.blockService, fid);
//    bPlusTree.T_BPLUS_TEST();
    //applicationContainer.testBlock();
   // applicationContainer.testQueryScanner(fid);
    return 0;
}

/** 

    metablock datablock

**/ 