//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_ONEINDEXQUERYSCANNERFACTORY_H
#define DB_ONEINDEXQUERYSCANNERFACTORY_H
#include "ColumnModel.h"
#include "OneIndexQueryScanner.h"

class OneIndexQueryScannerFactory{
public:
    static QueryScanner * CreateRangeIndexQueryScanner(
            ColumnModel::Type type,
            RecordService *recordService, BlockService *blockService, size_t len, int ifid, int tfid,
            void * value){
        if(type == ColumnModel::Type::Int){
            int v = *(int *)value;
            return new OneIndexQueryScanner<int>(
                    recordService, blockService, len, ifid, tfid,
                    v
            );
        }
        else if(type == ColumnModel::Type::Float){
            double v = *(double *)value;
            return new OneIndexQueryScanner<double>(
                    recordService, blockService, len, ifid, tfid,
                    v
            );
        }
        else {
            char * v = (char *)value;
            if(len <= 16){
                return new OneIndexQueryScanner<char[16]>(
                        recordService, blockService, len, ifid, tfid,
                        v
                );
            }
            else if( len <=64 ){
                return new OneIndexQueryScanner<char[64]>(
                        recordService, blockService, len, ifid, tfid,
                        v
                );
            }
            else{
                return new OneIndexQueryScanner<char[256]>(
                        recordService, blockService, len, ifid, tfid,
                        v
                );
            }
        }
    }
};

#endif //DB_ONEINDEXQUERYSCANNERFACTORY_H
