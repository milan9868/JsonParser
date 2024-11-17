#ifndef JSON_PRINTER_H
#define JSON_PRINTER_H

#include "json_parser.hpp"




class JsonValuePrinter {

public:

    std::string operator()(const JsonObject& obj);

    std::string operator()(const JsonArray& arr);

    std::string operator()(const std::string& str) const;

    std::string operator()(double num) const;

    std::string operator()(bool b) const;

    std::string operator()(nullptr_t) const;
};

#endif
