#ifndef JSON_PARSER_H
#define JSON_PARSER_H


#include <iostream>
#include <variant>
#include <unordered_map>
#include <vector>
#include <utility>
#include <stdexcept>
#include <fstream>
#include <sstream>



struct JsonValue;

using JsonObject = std::unordered_map<std::string, JsonValue>;
using JsonArray = std::vector<JsonValue>;

struct JsonValue {

    std::variant<JsonObject, JsonArray, std::string, int, bool, nullptr_t> value;

    JsonValue() {}
    JsonValue(const JsonObject& obj) : value(obj) {}
    JsonValue(const JsonArray& arr) : value(arr) {}
    JsonValue(const std::string& s) : value(s) {}
    JsonValue(int i) : value(i) {}
    JsonValue(bool b) : value(b) {}
    JsonValue(nullptr_t n) : value(n) {}

};


class JsonParser {

public:
    JsonParser(const std::string& source);

    JsonObject parse();

private:
    std::string source;
    int cursor;
    int line;
    int column;


    void moveCursorForward();
    char peek();
    void eatWhitespace();

    bool isWhitespace();

    bool cursorAtDigit();

    JsonObject json();

    JsonValue element();

    JsonValue value();
    JsonObject object();
    JsonObject members();
    std::pair<std::string, JsonValue> member();
    std::string string() ;
    int number() ;
    JsonArray array() ;
    JsonValue True() ;

    JsonValue False() ;
    JsonValue null() ;
    void syntaxError(const std::string& message);



};

#endif
