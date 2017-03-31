//
// Created by 张程易 on 2017/3/29.
//

#ifndef DB_RANGEINDEXQUERYSCANNERFACTORY_H
#define DB_RANGEINDEXQUERYSCANNERFACTORY_H

#include "ColumnModel.h"
#include "QueryScanner.h"
#include "RangeIndexQueryScanner.h"

class RangeIndexQueryScannerFactory{
public:
    static QueryScanner * CreateRangeIndexQueryScanner(
            ColumnModel::Type type,
            RecordService *recordService, BlockService *blockService, size_t len, int ifid, int tfid,
            void * left, void * right, bool leq, bool req, bool withLeft, bool withRight, std::string on
    ){
        if(type == ColumnModel::Type::Int){
            int l = left == nullptr ? 0 :*(int *)left;
            int r = right == nullptr ? 0 : *(int *)right;
            return new RangeIndexQueryScanner<int>(
                recordService, blockService, len, ifid, tfid,
                l, r, leq, req, withLeft, withRight, on
            );
        }
        else if(type == ColumnModel::Type::Float){
            double l = left == nullptr ? 0 : *(double *)left;
            double r = right == nullptr ? 0 :*(double *)right;
            return new RangeIndexQueryScanner<double>(
                    recordService, blockService, len, ifid, tfid,
                    l, r, leq, req, withLeft, withRight, on
            );
        }
        else {
            char * l = (char *)left;
            char * r = (char *)right;
            if(len <= 16){
               return new RangeIndexQueryScanner<char[16]>(
                       recordService, blockService, len, ifid, tfid,
                       l, r, leq, req, withLeft, withRight, on
               );
            }
            else if( len <=64 ){
                return new RangeIndexQueryScanner<char[64]>(
                        recordService, blockService, len, ifid, tfid,
                        l, r, leq, req, withLeft, withRight, on
                );
            }
            else{
                return new RangeIndexQueryScanner<char[256]>(
                        recordService, blockService, len, ifid, tfid,
                        l, r, leq, req, withLeft, withRight, on
                );
            }
        }
    }
};

#endif //DB_RANGEINDEXQUERYSCANNERFACTORY_H
