#include "json.h"

char parseStringBuffer[ 65536 ];

bool isEscapeChar(char chr){
    return chr == ' ' || chr == '\t' || chr == '\n';
}

bool isNumber( char chr){
    return (chr >= '0' && chr <= '9') || chr == '-' ;
}

bool isValidAttr( char chr){
    return (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z')
        || chr == '_' || chr == '$' || (chr >= '0' && chr <= '9');
}

JSON * parseObject(const char * str, size_t & pos);
JSON * parseArray(const char * str, size_t & pos);
JSON * parseNumber(const char * str, size_t & pos);
JSON * parseString(const char * str, size_t & pos);
JSON * parseValue(const char * str, size_t & pos);
JSON * parseConstant(const char * str, size_t & pos);


JSON * parseNumber(const char * str, size_t & pos){
    bool doubleFlag = false;
    bool negFlag = false;
    if(str[pos] == '-'){ negFlag = true; pos++; }
    long long integerBuffer = 0;
    double doubleBuffer;
    while( str[pos] >= '0' && str[pos] <= '9'){
        integerBuffer = integerBuffer * 10 + str[pos] - '0';
        pos ++;
    }
    doubleBuffer = integerBuffer;
    if( str[pos] == '.'){
        doubleFlag = true;
        double base = 1;
        pos ++;
        while( str[pos] >= '0' && str[pos] <= '9'){
            base /= 10;
            doubleBuffer += base * (str[pos] - '0');
            pos ++;
        }
    }
    if(str[pos] == 'e' || str[pos] == 'E'){
        doubleFlag = true;
        pos ++; // e | E 
        bool expNegFlag = false;
        int expBuffer = 0;
        if( str[pos] == '-' ){ expNegFlag = true; pos++;}
        else if( str[pos] == '+') {pos ++;}
        while( str[pos] >= '0' && str[pos] <= '9'){
            expBuffer = expBuffer * 10 + str[pos] - '0';
            pos ++;
        }
        if(expNegFlag) expBuffer = -expBuffer;
        doubleBuffer = doubleBuffer * pow(10, expBuffer);
    }
    if(negFlag) {
        integerBuffer = -integerBuffer;
        doubleBuffer = -doubleBuffer;
    }
    if(!doubleFlag) return new JSONInteger(integerBuffer);
    else return new JSONDouble(doubleBuffer);
}


JSON * parseArray(const char * str, size_t & pos){
    pos ++; // [
    JSONArray * jsonArray = new JSONArray();
    while( isEscapeChar(str[pos]) ) pos ++;
    while( str[pos] != ']' ){
        JSON * value = parseValue(str, pos);
        if( value == nullptr ) { delete jsonArray; return nullptr; }
        jsonArray->elements.emplace_back(value);
        while( isEscapeChar(str[pos]) ) pos ++;
        if( str[pos] != ']' && str[pos] != ','){ delete jsonArray; return nullptr; }
        if (str[pos] == ',') pos++;
        while( isEscapeChar(str[pos]) ) pos ++;
    }
    pos ++; // ]
    return (JSON *) jsonArray;
}

size_t parseStringLiteral(const char * str, size_t & pos){
    size_t now = 0;
    pos ++; // "
    while( str[pos] != '\"' && str[pos] != 0){
        if( str[pos] == '\\'){
            switch( str[pos + 1] ){
                case '\"': parseStringBuffer[ now ++ ] = '\"'; break;
                case '\\': parseStringBuffer[ now ++ ] = '\\'; break;
                case '/': parseStringBuffer[ now ++ ] = '/'; break;
                case 'b': parseStringBuffer[ now ++ ] = '\b'; break;
                case 'f': parseStringBuffer[ now ++ ] = '\f'; break;
                case 'n': parseStringBuffer[ now ++ ] = '\n'; break;
                case 'r': parseStringBuffer[ now ++ ] = '\r'; break;
                case 't': parseStringBuffer[ now ++ ] = '\t'; break;
                default:
                    return 0;
            }
            pos += 2;
        }
        else{
            parseStringBuffer[ now ++ ] = str[ pos++ ];
        }
    }
    if( str[pos] == 0 ) return 0;
    pos ++; // "
    return now + 1;
}

JSON * parseString( const char * str, size_t & pos){
    size_t end = parseStringLiteral( str, pos );
    if( end == 0 ) return nullptr;
    return new JSONString( parseStringBuffer , end - 1);
}

JSON * parseConstant(const char * str, size_t & pos){
    if( memcmp(str + pos, "true", 4) == 0 ) {
        pos += 4;
        return new JSONBoolean(true);
    }
    if( memcmp(str + pos, "false", 5) == 0) {
        pos += 5;
        return new JSONBoolean(false);
    }
    if( memcmp(str + pos, "null", 4) == 0){
        pos += 4;
        return new JSONNull();
    }
    return nullptr;
}

JSON * parseValue(const char * str, size_t & pos){
    if ( str[pos] == '{') return parseObject(str, pos);
    else if( str[pos] == '[') return parseArray(str, pos);
    else if( str[pos] == '\"') return parseString(str, pos);
    else if( isNumber( str[pos])) return parseNumber(str, pos);
    else return parseConstant(str, pos);
}

JSON * parseObject(const char * str, size_t & pos){
    JSONObject * jsonObject = new JSONObject();
    pos ++; // {
    while( isEscapeChar(str[pos]) ) pos ++;
    while( str[pos] != '}' ){
        size_t keyOffset = parseStringLiteral(str, pos);
        if( keyOffset == 0 ) { delete jsonObject; return nullptr; }
        std::string key( parseStringBuffer , keyOffset - 1 );
    
        while( isEscapeChar(str[pos]) ) pos ++;
        if( str[pos] != ':') { delete jsonObject; return nullptr; }
        pos ++; // :
        while( isEscapeChar(str[pos]) ) pos ++;

        JSON * value = parseValue(str, pos);
        if( value == nullptr ) { delete jsonObject; return nullptr; }
        jsonObject->hashMap.emplace( std::move(key), value );

        while( isEscapeChar(str[pos]) ) pos ++;

        if( str[pos] != '}' && str[pos] != ','){ delete jsonObject; return nullptr; }
        if (str[pos] == ',') pos++;
        while( isEscapeChar(str[pos]) ) pos ++;
    }
    pos ++ ; // }
    return (JSON *) jsonObject;
}


JSON * parsePath(const char * path, size_t & pos, JSON * root){
    if(path[pos] == 0 || root == nullptr) return root;
    if(path[pos] == '.'){
        size_t now = 0;
        pos ++; // .
        while(isValidAttr(path[pos])){
            parseStringBuffer[now ++ ] = path[pos ++];
        }
        parseStringBuffer[now] = 0;
        return parsePath(path, pos, root->get(parseStringBuffer));
    }
    else if(path[pos] == '['){
        pos++; // [
        if(path[pos] == '\"'){
            pos ++; // "
            const char * head = path + pos;
            size_t result = parseStringLiteral(path, pos);
            if( result == 0 || path[pos] != ']' )return nullptr;
            pos ++; // ]
            return parsePath(path, pos, root->get(std::string(head, result)));
        }
        else{
            long long integerBuffer = 0;
            while( path[pos] >= '0' && path[pos] <= '9'){
                integerBuffer = integerBuffer * 10 + path[pos] - '0';
                pos ++;
            }
            if(path[pos] != ']') return nullptr;
            pos ++;
            return parsePath(path, pos, root->get(integerBuffer));
        }
    }
    return nullptr;
}

JSON * JSON::path(const std::string &path){
    return this->path(path.c_str());
}

JSON * JSON::path(const char * path){
    size_t pos = 0;
    return parsePath( path, pos, this);
}

JSON * JSON::parse(const char * str){
    size_t pos = 0;
    while( isEscapeChar(str[pos]) ) pos ++;
    return parseObject(str, pos);
}

