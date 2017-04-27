//
// Created by 张程易 on 2017/4/8.
//

#include "SQLCondition.h"



JSON * SQLCondition::toJSON(TableModel * tableModel){
    auto json = new JSONObject();
    json->set("type", TypeName[ static_cast<unsigned>(type)]);
    json->set("cid", tableModel->getColumn(cid)->getName());
    json->set("value", ColumnTypeUtil::toJSON(tableModel->getColumn(cid)->getType(), value));
    return json;
}

SQLCondition::SQLCondition(TableModel * tableModel, SQLCondition::Type type, int cid, void *value) : type(type), cid(cid), value(value) {
    this->on = tableModel->getColumn(cid)->getOn();
    this->columnSize = tableModel->getColumn(cid)->getSize();
    this->columnType = tableModel->getColumn(cid)->getType();
}

bool SQLCondition::filter(void *data) {
    switch(type){
        case Type::eq:
        TypeUtil::
    }
}
