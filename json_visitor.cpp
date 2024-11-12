#include "json_parser.hpp"
#include "json_visitor.hpp"




JsonValueVisitor::JsonValueVisitor(std::string& jsonexpr): jsonExpression(jsonexpr), position(0) {}


JsonValue JsonValueVisitor::visit(const JsonValue& json_value) {
    return std::visit(*this, json_value.value);
}

JsonValue JsonValueVisitor::operator()(const JsonObject& obj) {

    if(!called) {
        called = true;
        root = obj;
    }


    std::string key = getKey();
    if (key == "") {
        return obj;
    }

    auto tmp = obj.find(key);
    if (tmp == obj.end()) {
        std::cerr << "Non existant key " + key + "\n";
        exit(EXIT_FAILURE);
    }


    return visit(tmp->second);
}

JsonValue JsonValueVisitor::operator()(const JsonArray& arr) {
    int index = getArrayIndex();
    if (index == -1) {
        return arr;
    }

    if (index < 0 || index >= arr.size()) {
        std::cerr << "Array index out of bounds\n";
        exit(EXIT_FAILURE);
    }

    // Use visit() instead of direct std::visit
    return visit(arr[index]);
}

JsonValue JsonValueVisitor::operator()(const std::string& str) const {
    return str;
}

JsonValue JsonValueVisitor::operator()(int num) const {
    return num;
}

JsonValue JsonValueVisitor::operator()(bool b) const {
    return b;
}

JsonValue JsonValueVisitor::operator()(nullptr_t) const {
    return nullptr;
}



std::string JsonValueVisitor::getKey() {

    std::string key;

    while (position < jsonExpression.length() && jsonExpression[position] != '.' && jsonExpression[position] != '[') {

        key += jsonExpression[position];
        position++;
    }

    position++; // consume dot or [
    return key;
}

int JsonValueVisitor::getArrayIndex() {

    int start_position = position;




    while(position < jsonExpression.length() && jsonExpression[position] != ']') {

        position++;
    }
    if (position == start_position) {
        return -1;
    }

    if (!std::isdigit(jsonExpression[start_position])) {
        position = start_position;
        int index = getArrayIndexNested();
        return index;
    }

    position++; // consume ']'
    if (position < jsonExpression.length() - 1) {
        position++; // consume dot
    }
    return std::stoi(jsonExpression.substr(start_position, position - start_position));
}

int JsonValueVisitor::getArrayIndexNested() {

    int bracket_counter = 1;

    std::string new_expression;

    while (position < jsonExpression.length() && bracket_counter > 0) {

        if (jsonExpression[position] == ']')
            bracket_counter--;

        if (jsonExpression[position] == '[')
            bracket_counter++;

        if (bracket_counter > 0)
            new_expression += jsonExpression[position];

        position++;

    }

    if (position < jsonExpression.length() - 1) {
        position++; // consume dot
    }

    JsonValueVisitor visitor(new_expression);
    int x = std::get<int>(visitor.visit(root).value);
    return x;
}
