#include "json_parser.hpp"



JsonParser::JsonParser(const std::string& source) {
    cursor = 0;
    line = 1;
    column = 0;
    this->source = source;
}

JsonObject JsonParser::parse() {
    return json();
}



void JsonParser::moveCursorForward() {
    cursor++;
    column++;
}

char JsonParser::peek() {
    return source[cursor+1];
}

void JsonParser::eatWhitespace() {
    while(isWhitespace()) {
        moveCursorForward();
    }
}

bool JsonParser::isWhitespace() {
    if (source[cursor] == ' ' || source[cursor] == '\t' ) {
        return true;
    }
    if (source[cursor] == '\n') {
        line++;
        column = 0;
        return true;
    }

    return false;
}

bool JsonParser::cursorAtDigit() {
    return source[cursor] >= '0' && source[cursor] <= '9';
}

JsonObject JsonParser::json() {
    JsonValue jv = element();

    return std::get<JsonObject>(jv.value);
}

JsonValue JsonParser::element() {

    eatWhitespace();
    JsonValue jv = value();
    //if (jv == nullptr) { return false;}
    eatWhitespace();

    return jv;

}

JsonValue JsonParser::value() {

    switch (source[cursor]) {

        case '{':
            return object();

        case '[':
            return array();

        case '"':
            return string();

        default:
            if (source[cursor] == 't') { return True();}
            if (source[cursor] == 'f') { return False();}
            if (source[cursor] == 'n') { return null();}

            return number();

    }
}

JsonObject JsonParser::object() {
    moveCursorForward(); // consume {

    if (source[cursor] == '}') {
        moveCursorForward();
        return JsonObject();
    }


    JsonObject jo = members();

    eatWhitespace();

    if (source[cursor] != '}') { syntaxError("missing }");}

    moveCursorForward(); // consume }

    return jo;
}


JsonObject JsonParser::members() {
    //if (!member()) { return false;}

    JsonObject jo;
    jo.insert(member());

    while (source[cursor] == ',') {
        moveCursorForward(); // consume ,
        jo.insert(member());
    }

    return jo;

}

std::pair<std::string, JsonValue> JsonParser::member() {
    eatWhitespace();
    std::string key = string();
    eatWhitespace();
    if (!(source[cursor] == ':')) { syntaxError("missing :");} // bacacemo gresku
    moveCursorForward();
    JsonValue jv = element();

    return {key, jv};
}

std::string JsonParser::string() {
    if (source[cursor] != '"') { syntaxError("missing ,");}
    moveCursorForward();
    int inital_position = cursor;
    while (cursor < source.length() && source[cursor] != '"') { // mora se dodati podrska za /" bekslesovan navodnik
        moveCursorForward();
    }

    moveCursorForward(); // consume second "



    return source.substr(inital_position,cursor-inital_position-1);
}


double JsonParser::number() {

    int initial_position = cursor;

    if (source[cursor] == '-') {
        moveCursorForward();
    }

    while (cursorAtDigit()) {
        moveCursorForward();
    }

    if (source[cursor] == '.') {
        moveCursorForward();
        while (cursorAtDigit()) {
            moveCursorForward();
        }
    }

    // scientific notation
    if (source[cursor] == 'e' || source[cursor] == 'E') {
        moveCursorForward();

        if (source[cursor] == '+' || source[cursor] == '-') {
            moveCursorForward();
        }
        while (cursorAtDigit()) {
            moveCursorForward();
        }
    }

    return std::stod(source.substr(initial_position, cursor-initial_position));
}

JsonArray JsonParser::array() {

    moveCursorForward(); // consume [

    JsonArray arr;

    if (source[cursor] == ']') {
        return arr;
    }

    arr.push_back(element());
    while (source[cursor] == ',') {
        moveCursorForward(); // consume ,
        arr.push_back(element());
    }

    if (source[cursor] != ']') { syntaxError("missing ]");}
    moveCursorForward(); // consume ]

    return arr;
}


JsonValue JsonParser::True() {

    if (source.compare(cursor,4, "true") == 0) {

        cursor += 4;
        return JsonValue(true);
    }

    syntaxError("invalid literal");

    return JsonValue(nullptr);

}

JsonValue JsonParser::False() {

    if (source.compare(cursor,5, "false") == 0) {

        cursor += 5;
        return JsonValue(false);
    }

    syntaxError("invalid literal");

    return JsonValue(nullptr);


}

JsonValue JsonParser::null() {

    if (source.compare(cursor,4, "null") == 0) {

        cursor += 4;
        return JsonValue(nullptr);
    }

    syntaxError("invalid literal");

    return JsonValue(nullptr);
}



void JsonParser::syntaxError(const std::string& message) {

    throw std::runtime_error("Syntax error at line " + std::to_string(line) + ", column " + std::to_string(column) + ": " + source.substr(cursor) + "\n" + message);
}





