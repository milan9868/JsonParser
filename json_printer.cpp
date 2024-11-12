#include "json_printer.hpp"


std::string JsonValuePrinter::operator()(const JsonObject& obj) {

    std::string result = "{ ";
    bool first = true;

    for (const auto& [key, val] : obj) {

        if (!first) { result += ", ";}

        result += "\"" + key + "\": " + std::visit(*this,val.value);
        first = false;
    }

    result += " }";
    return result;

}

std::string JsonValuePrinter::operator()(const JsonArray& arr) {

    std::string result = "[ ";

    for (size_t i = 0; i < arr.size(); ++i) {

        result += std::visit(*this,arr[i].value);

        if (i < arr.size() - 1) result += ", ";

    }

    result += " ]";
    return result;

}

std::string JsonValuePrinter::operator()(const std::string& str) const {
    return str;
}

std::string JsonValuePrinter::operator()(int num) const {

    return std::to_string(num);

}

std::string JsonValuePrinter::operator()(bool b) const {

    if(b) {

        return "true";
    }

    return "false";

}

std::string JsonValuePrinter::operator()(nullptr_t) const {

    return "null";
}
