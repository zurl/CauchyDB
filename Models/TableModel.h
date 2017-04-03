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
    size_t len;
    std::map<int, IndexModel> indices;
    std::map<int, size_t> lenTable;
    std::map<std::string, int> keyindex;
public:
    TableModel(FileService * fileService, std::string && name, JSON * config);

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

    inline int getColumnIndex(const std::string & str) const {
        auto iter = keyindex.find(str);
        if( iter == keyindex.end()) throw SQLExecuteException(1, "unknown column");
        return iter->second;
    }

    inline size_t getLen() const {
        return len;
    }

    inline const std::vector<ColumnModel> &getColumns() {
        return columns;
    }
    inline size_t getColumnLen(int cid){
        return lenTable[cid];
    }

    inline std::map<int, IndexModel> * getIndices() {
        return &indices;
    }

    JSON * toJSON();
};

#endif //DB_TABLEMODEL_H
