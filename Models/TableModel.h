//
// Created by 张程易 on 2017/3/31.
//

#ifndef DB_TABLEMODEL_H
#define DB_TABLEMODEL_H

#include "ColumnModel.h"
#include "../Common.h"
#include "../Services/FileService.h"
#include "IndexModel.h"

class TableModel{
    int fid;
    FileService * fileService;
    std::string name;
    std::vector<ColumnModel> columns;
    int len;
    std::map<int, IndexModel> indices;
    std::map<int, int> lenTable;
    std::map<std::string, int> keyindex;
    BlockService * blockService;
public:
    TableModel(FileService * fileService, BlockService * blockService, std::string && name, JSON * config, bool create);

    IndexModel * findIndexOn(int cid)  ;

    inline ColumnModel * getColumn(int cid) {
        return &(columns.operator[](cid));
    }

    inline int getFid() const {
        return fid;
    }

    inline const std::string &getName() const {
        return name;
    }

    int getColumnIndex(const std::string & str) const ;

    inline int getLen() const {
        return len;
    }

    inline const std::vector<ColumnModel> &getColumns() {
        return columns;
    }
    inline int getColumnLen(int cid){
        return lenTable[cid];
    }

    inline std::map<int, IndexModel> * getIndices() {
        return &indices;
    }

    JSON * toJSON();
};

#endif //DB_TABLEMODEL_H
