#include "json_parser.hpp"
#include "json_printer.hpp"
#include "json_expression.hpp"
#include "json_visitor.hpp"



std::string readFile(const std::string& filename) {

    std::ifstream file(filename);

    if (!file.is_open()) {

        std::cerr << "Cant open " + filename + "\n";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}


int main(int argc, char** argv) {

    if (argc < 3) {
        std::cerr << "Missing command line arguments\n";
        return 1;
    }


    std::string source = readFile(argv[1]);

    JsonParser jp(source);

    try {
        JsonObject jo = jp.parse();
        JsonExpression jsonExpression(jo,argv[2]);
        std::cout << jsonExpression.evaluate() << '\n';


    }
    catch (std::runtime_error &e) {
        std::cout << "jesam li ovde";
        std::cout << e.what() << '\n';
    }


    return 0;
}
