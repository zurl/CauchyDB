//
// Created by 张程易 on 2017/4/8.
//

#include "SQLCondition.h"


SQLCondition::SQLCondition(Type type, int cid, void *value) : type(type), cid(cid), value(value) {}

JSON * SQLCondition::toJSON(TableModel * tableModel){
    auto json = new JSONObject();
    json->set("type", TypeName[ static_cast<unsigned>(type)]);
    json->set("cid", tableModel->getColumn(cid)->getName());
    json->set("value", ColumnTypeUtil::toJSON(tableModel->getColumn(cid)->getType(), value));
    return json;
}
