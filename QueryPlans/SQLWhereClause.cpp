//
// Created by 张程易 on 2017/4/8.
//

#include "SQLWhereClause.h"

void SQLWhereClause::addCondition(AbstractSQLCondition * cond){
    conds.emplace_back(cond);
}
JSON * SQLWhereClause::toJSON(TableModel * tableModel){
    auto json = new JSONArray();
    for(auto x: conds){
        json->put(x->toJSON(tableModel));
    }
    return json;
}

bool SQLWhereClause::filter(void *data) {
    for(auto &x : conds){
        if(!x->filter(data))return false;
    }
    return true;
}

