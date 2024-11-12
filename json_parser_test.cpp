#include <gtest/gtest.h>
#include <fstream>
#include "json_parser.hpp"
#include "json_expression.hpp"
#include "json_printer.hpp"
#include "json_visitor.hpp"


// Basic JSON Parser Tests
class JsonParserTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(JsonParserTest, EmptyObject) {
    JsonParser parser("{}");
    JsonObject obj = parser.parse();
    EXPECT_EQ(obj.size(), 0);
}

TEST_F(JsonParserTest, SimpleObject) {
    JsonParser parser("{\"key\": 1}");
    JsonObject obj = parser.parse();
    EXPECT_EQ(obj.size(), 1);
    EXPECT_EQ(std::get<int>(obj["key"].value), 1);
}

TEST_F(JsonParserTest, SyntaxErrorTest) {
    JsonParser parser(R"({"qwe" 1: , ,})");

    // Use EXPECT_THROW to check that a specific exception is thrown
    EXPECT_THROW({
        parser.parse();
    }, std::runtime_error);
}

// Expression Evaluation Tests
class JsonExpressionTest : public ::testing::Test {
protected:
    JsonObject setupTestObject() {
        std::string json = R"({
            "number": 42,
            "array": [1, 2, 3, 4, 5],
            "nested": {
                "value": 10
            }
        })";
        JsonParser parser(json);
        return parser.parse();
    }
};


TEST_F(JsonExpressionTest, SimpleTest) {

    JsonObject obj = setupTestObject();
    int num = JsonExpression(obj,"array[1]+array[2]").evaluate();

    int size = JsonExpression(obj,"size(nested)").evaluate();

    EXPECT_EQ(size,1);
    EXPECT_EQ(num,5);
}

TEST_F(JsonExpressionTest, NestedTest) {

    JsonObject obj = setupTestObject();
    int num = JsonExpression(obj,"array[array[1]]").evaluate();


    EXPECT_EQ(num,1);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
