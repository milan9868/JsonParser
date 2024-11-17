#ifndef JSON_VALUE_VISITOR_H
#define JSON_VALUE_VISITOR_H


#include "json_parser.hpp"


class JsonValueVisitor {
public:

    JsonValueVisitor(std::string& jsonexpr);



    // This is the main entry point for visitation
    JsonValue visit(const JsonValue& json_value);

    JsonValue operator()(const JsonObject& obj);

    JsonValue operator()(const JsonArray& arr);

    JsonValue operator()(const std::string& str) const ;

    JsonValue operator()(double num) const;

    JsonValue operator()(bool b) const;

    JsonValue operator()(nullptr_t) const;

private:
    std::string jsonExpression;
    int position;
    bool called = false;
    JsonValue root;

    std::string getKey();

    int getArrayIndex();

    int getArrayIndexNested();
};


#endif
