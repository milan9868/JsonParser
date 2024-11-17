#ifndef JEXPRESSION_H
#define JEXPRESSION_H

#include <stack>
#include <limits>
#include "json_printer.hpp"
#include "json_visitor.hpp"

class JsonExpression {
public:
    JsonExpression(JsonObject &obj, std::string expr);

    double evaluate();

private:
    JsonObject& object;
    std::string jsonExpression;

    std::vector<std::string> toPostfix();
    bool isOperator(char c);
    int precedence(char c);
    bool isSimpleExpression();
    bool isValidVariableChar(char c);
};

#endif // JEXPRESSION_H
