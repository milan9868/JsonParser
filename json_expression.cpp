#include "json_expression.hpp"



JsonExpression::JsonExpression(JsonObject &obj, std::string expr): object(obj), jsonExpression(expr) {}

double JsonExpression::evaluate() {


    if (isSimpleExpression()) {

        JsonValueVisitor visitor(jsonExpression);
        JsonValue result = visitor(object);
        std::cout << std::visit(JsonValuePrinter{}, result.value) << '\n';
        exit(EXIT_SUCCESS);
    }

    std::vector<std::string> postfix = toPostfix();

    std::stack<double> stack;

    for (const std::string& token : postfix) {
        // If token is a number
        if (isdigit(token[0]) || (token[0] == '-' && token.length() > 1)) {
            stack.push(std::stoi(token));
        }
        // If token is a min/max function with argument count (e.g., "m3" or "M2")
        else if (token[0] == 'm' || token[0] == 'M') {
            int argCount = std::stoi(token.substr(1));
            double result;

            // For min function
            if (token[0] == 'm') {
                result = stack.top();
                stack.pop();
                for (int i = 0; i < argCount; i++) {
                    result = std::min(result, stack.top());
                    stack.pop();
                }
            }
            // For max function
            else {
                result = stack.top();
                stack.pop();
                for (int i = 0; i < argCount; i++) {
                    result = std::max(result, stack.top());
                    stack.pop();
                }
            }
            stack.push(result);
        }
        // Regular operators
        else {
            double b = stack.top(); stack.pop();
            double a = stack.top(); stack.pop();

            switch (token[0]) {
                case '+': stack.push(a + b); break;
                case '-': stack.push(a - b); break;
                case '*': stack.push(a * b); break;
                case '/': stack.push(a / b); break;
            }
        }
    }

    return stack.top();

}




std::vector<std::string> JsonExpression::toPostfix() {

    std::vector<std::string> output;
    std::stack<char> operator_stack;
    std::stack<int> arg_counts;

    int i = 0;
    while (i < jsonExpression.size()) {
        char token = jsonExpression[i];

        if (token == ' ') {
            i++;
            continue;

        }

        if (std::isdigit(token)) {

            std::string number;
            while (i < jsonExpression.size() && std::isdigit(jsonExpression[i])) {
                number += jsonExpression[i++];
            }
            output.push_back(number); // Add number to the output queue
            continue; // Skip incrementing `i` since it's done in the loop above

        }
        else if (isOperator(token)) {
            // Handle operators
            while (!operator_stack.empty() && operator_stack.top() != '(' &&
                (precedence(operator_stack.top()) > precedence(token))) {

                output.push_back(std::string(1, operator_stack.top()));
            operator_stack.pop();
                }
                operator_stack.push(token);

        }
        else if (token == ',') { // Handle function argument separators

            if (!arg_counts.empty()) {
                arg_counts.top()++;
            }

            while (!operator_stack.empty() && operator_stack.top() != '(') {
                output.push_back(std::string(1, operator_stack.top()));
                operator_stack.pop();
            }

        }
        else if (token == '(') {

            operator_stack.push(token);
        }
        else if (token == ')') {

            while (!operator_stack.empty() && operator_stack.top() != '(') {
                output.push_back(std::string(1, operator_stack.top()));
                operator_stack.pop();

            }
            if (!operator_stack.empty() && operator_stack.top() == '(') {

                operator_stack.pop();

            }

            if (!operator_stack.empty() && (operator_stack.top() == 'm' || operator_stack.top() == 'M')) {

                char function = operator_stack.top();
                operator_stack.pop();

                if (!arg_counts.empty()) {

                    output.push_back(std::string(1, function) + std::to_string(arg_counts.top()));
                    arg_counts.pop();

                }
                else {

                }
            }
        }
        else if (jsonExpression.substr(i, 3) == "min") {
            operator_stack.push('m'); // Use 'm' as the symbol for min
            i += 2; // Skip next two characters ("in")
            arg_counts.push(0);
        }
        else if (jsonExpression.substr(i, 3) == "max") {
            operator_stack.push('M'); // Use 'M' as the symbol for max
            i += 2; // Skip next two characters ("ax")
            arg_counts.push(0);
        }
        else if (jsonExpression.substr(i, 4) == "size") {
            operator_stack.push('S'); // Use 'S' as the symbol for size
            i += 3; // Skip next three characters ("ize")
        }
        else {
            std::string variable;
            while (i < jsonExpression.size() &&
                (std::isalnum(jsonExpression[i]) || jsonExpression[i] == '.' || jsonExpression[i] == '[' || jsonExpression[i] == ']')) {
                variable += jsonExpression[i++];
                }

            JsonValueVisitor visitor(variable);
            JsonValue value = visitor(object);

            if (std::holds_alternative<JsonObject>(value.value) || std::holds_alternative<JsonArray>(value.value) || std::holds_alternative<std::string>(value.value)) {
                if (operator_stack.empty() || operator_stack.top() != '(') {
                    std::cerr << "Invalid expression\n";
                    exit(EXIT_FAILURE);
                }

                operator_stack.pop(); // pop (
                char func_symbol = operator_stack.top();

                switch (func_symbol) {
                    case 'm': {
                        operator_stack.pop();
                        if (std::holds_alternative<JsonArray>(value.value)) {

                            const JsonArray& arr = std::get<JsonArray>(value.value);

                            if (arr.empty()) {
                                std::cerr << "Cannot find minimum of empty array\n";
                                exit(EXIT_FAILURE);
                            }
                            // Find minimum value in array
                            int min_val = std::numeric_limits<int>::max();
                            for (const auto& elem : arr) {
                                if (std::holds_alternative<int>(elem.value)) {
                                    min_val = std::min(min_val, std::get<int>(elem.value));
                                }
                            }
                            output.push_back(std::to_string(min_val));
                            i++; // consume )
                            continue;
                        } else {
                            std::cerr << "Min operation only supported on arrays\n";
                            exit(EXIT_FAILURE);
                        }

                    }

                    case 'M': {
                        operator_stack.pop();
                        if (std::holds_alternative<JsonArray>(value.value)) {

                            const JsonArray& arr = std::get<JsonArray>(value.value);

                            if (arr.empty()) {
                                std::cerr << "Cannot find maximum of empty array\n";
                                exit(EXIT_FAILURE);
                            }
                            // Find maximum value in array
                            int max_val = std::numeric_limits<int>::min();
                            for (const auto& elem : arr) {
                                if (std::holds_alternative<int>(elem.value)) {
                                    max_val = std::max(max_val, std::get<int>(elem.value));
                                }
                            }
                            output.push_back(std::to_string(max_val));
                            i++; // consume )
                            continue;
                        } else {
                            std::cerr << "Max operation only supported on arrays\n";
                            exit(EXIT_FAILURE);
                        }

                    }

                    case 'S': {
                        operator_stack.pop();
                        if (std::holds_alternative<JsonArray>(value.value)) {
                            const JsonArray& arr = std::get<JsonArray>(value.value);
                            int tmp = arr.size();
                            output.push_back(std::to_string(tmp));
                        } else if (std::holds_alternative<JsonObject>(value.value)) {
                            const JsonObject& obj = std::get<JsonObject>(value.value);
                            int tmp = obj.size();
                            output.push_back(std::to_string(tmp));
                        } else if (std::holds_alternative<std::string>(value.value)) {
                            const std::string& str = std::get<std::string>(value.value);
                            int tmp = str.length();
                            output.push_back(std::to_string(tmp));
                        }
                        i++; // consume )
                        continue;
                    }
                }
            }




            int number = std::get<int>(value.value);
            output.push_back(std::to_string(number));
            continue;
        }

        i++; // Move to the next character
    }

    // Pop remaining operators in the stack to the output queue
    while (!operator_stack.empty()) {
        char op = operator_stack.top();
        operator_stack.pop();
        if (op == '(' || op == ')') {
            throw std::invalid_argument("Mismatched parentheses in expression.");
        }
        output.push_back(std::string(1, op));
    }


    return output;
}




bool JsonExpression::isOperator(char c) {

    return c == '+' || c == '-' || c == '*' || c == '/';
}

int JsonExpression::precedence(char c) {

    if (c == '+' || c == '-') {
        return 1;
    }

    if (c == '*' || c == '/') {
        return 2;
    }

    return 3;


}

bool JsonExpression::isSimpleExpression() {

    if (jsonExpression.find("min(") != std::string::npos) { return false;}

    if (jsonExpression.find("max(") != std::string::npos) { return false;}

    if (jsonExpression.find("size(") != std::string::npos) { return false;}

    std::string operators = "+-*/";
    for (char c : operators) {
        if (jsonExpression.find(c) != std::string::npos) { return false;}
    }

    return true;
}
