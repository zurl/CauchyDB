#include "JSON.h"

std::string getIndent(int indent){
    std::string result = "";
    for(int i = 0; i < indent; i ++ )
        result += "  ";
    return result;
}

JSONString::JSONString( const std::string & str ) : str(str){}
JSONString::JSONString( const char * str ) : str(str){}

JSONString::JSONString( const char * str , int n ): str(str, n){}
std::string JSONString::toString(bool format, int indent)  {
    return "\"" + str + "\"";
}
JSON::Type JSONString::type() { return JSON::Type::String; }

JSONInteger::JSONInteger( long long value ): value(value){}
std::string JSONInteger::toString(bool format, int indent) {
    std::ostringstream os;
    os << value;
    return os.str();
}
JSON::Type JSONInteger::type() { return JSON::Type::Integer; }

JSONDouble::JSONDouble( double value) : value( value ){}
std::string JSONDouble::toString(bool format, int indent) {
    std::ostringstream os;
    os << value;
    return os.str();
}
JSON::Type JSONDouble::type() { return JSON::Type::Double; }

JSONObject::JSONObject(){}
JSONObject::~JSONObject(){
    for( auto & pair : hashMap ){
        delete pair.second;
    }
}
std::string JSONObject::toString(bool format, int indent) {
    std::ostringstream os;
    os << "{"; 
    if( format && hashMap.size() != 0) os << std::endl;
    int index = hashMap.size();
    for( auto & pair : hashMap){
        index --;
        if( format ) os << getIndent(indent + 1);
        os << "\"" << pair.first << "\"" << ":";
        if( format ) os << " ";
        os << pair.second->toString(format, indent + 1);
        if(index) os << ",";
        if( format ) os << std::endl;
    }
    if( format ) os << getIndent(indent);
    os << "}";
    return os.str();
}

JSON::Type JSONObject::type() { return JSON::Type::Object; }

const std::unordered_map<std::string, JSON *> &JSONObject::getHashMap() const {
    return hashMap;
}

JSONBoolean::JSONBoolean(bool value) : value( value ){}
std::string JSONBoolean::toString(bool format, int indent) {
    if( value ) return "true";
    else return "false";
}
JSON::Type JSONBoolean::type() { return JSON::Type::Boolean; }

JSONArray::~JSONArray(){
    for( auto & item : elements ){
        delete item;
    }
}
std::string JSONArray::toString(bool format, int indent) {
    std::ostringstream os;
    os << "["; 
    if( format && elements.size() != 0) os << std::endl;
    int index = elements.size();
    for( auto & item : elements){
        index --;
        if( format ) os << getIndent(indent + 1);
        os << item->toString(format, indent + 1);
        if(index)os << ",";
        if( format ) os << std::endl;
    }
    if( format ) os << getIndent(indent);
    os << "]";
    return os.str();
}
JSON::Type JSONArray::type() { return JSON::Type::Array; }

std::string JSONNull::toString(bool format, int indent) {
    return "null";
}
JSON::Type JSONNull::type() { return JSON::Type::Null; }

JSON * JSON::get(int index){
    if( this->type() != JSON::Type::Array) return nullptr;
    JSONArray * jsonArray = (JSONArray *)this;
    if(index >= jsonArray->getElements().size()) return nullptr;
    return jsonArray->getElements()[index];
}

JSON * JSON::get(const std::string & index){
    if( this->type() != JSON::Type::Object) return nullptr;
    JSONObject * jsonObject = (JSONObject *)this;
    auto iter = jsonObject->getHashMap().find(index);
    if( iter == jsonObject->getHashMap().end()) return nullptr;
    else return iter->second;
}

JSON * JSON::get(const char * index){
    return this->get(std::string(index));
}

JSONInteger * JSON::toInteger(){
    if( this->type() != JSON::Type::Integer) return nullptr;
    JSONInteger * jsonInteger = (JSONInteger *)this;
    return jsonInteger;
}

JSONDouble * JSON::toDouble(){
    if( this->type() != JSON::Type::Double) return nullptr;
    JSONDouble * jsonDouble = (JSONDouble *)this;
    return jsonDouble;
}

JSONObject * JSON::toObject(){
    if( this->type() != JSON::Type::Object) return nullptr;
    JSONObject * jsonObject = (JSONObject *)this;
    return jsonObject;
}

JSONBoolean * JSON::toBoolean(){
    if( this->type() != JSON::Type::Boolean) return nullptr;
    JSONBoolean * jsonBoolean = (JSONBoolean *)this;
    return jsonBoolean;
}

JSONArray * JSON::toArray(){
    if( this->type() != JSON::Type::Array) return nullptr;
    JSONArray * jsonArray = (JSONArray *)this;
    return jsonArray;
}

char jsonBuffer [ JSON_BUFFER_SIZE ];
JSON * JSON::fromFile(const char * fileName){
    FILE * file = fopen(fileName, "r");
    if( !file )return nullptr;
    int now = 0;
    int c = 0;
    c = fgetc(file);
    while(c != EOF){jsonBuffer[now++] = (char)c; c= fgetc(file);}
    jsonBuffer[now] = 0;
    JSON * json = JSON::parse(jsonBuffer);
    fclose(file);
    return json;
}

bool JSON::saveIntoFile(const char * fileName, bool format){
    FILE * file = fopen(fileName, "w");
    if( !file )return false;
    fputs(this->toString(format).c_str(), file);
    fclose(file);
    return true;
}

JSONString *JSON::toJString() {
    if( this->type() != JSON::Type::String) return nullptr;
    JSONString * jsonString = (JSONString *)this;
    return jsonString;
}
