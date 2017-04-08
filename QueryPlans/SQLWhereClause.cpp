//
// Created by 张程易 on 2017/4/8.
//

#include "SQLWhereClause.h"

void SQLWhereClause::addCondition(SQLCondition * cond){
    conds.emplace_back(cond);
}
JSON * SQLWhereClause::toJSON(TableModel * tableModel){
    auto json = new JSONArray();
    for(auto * x: conds){
        json->elements.emplace_back(x->toJSON(tableModel));
    }
    return json;
}

