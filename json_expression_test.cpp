
#include <gtest/gtest.h>
#include "json_expression.hpp"

// Test suite for JsonExpression
class JsonExpressionTest : public ::testing::Test {
protected:
    JsonObject obj;
    JsonExpression* expr;

    void SetUp() override {
        obj["a"] = JsonValue(5);
        expr = new JsonExpression(obj, "a + 3");
    }

    void TearDown() override {
        delete expr;
    }
};

// Constructor Test
TEST_F(JsonExpressionTest, ConstructorInitialization) {
    EXPECT_NO_THROW(JsonExpression(obj, "a + 3"));
}

// Evaluate Method Tests - Simple Expression
TEST_F(JsonExpressionTest, EvaluateSimpleExpression) {
    JsonExpression simpleExpr(obj, "2 + 3");
    EXPECT_EQ(simpleExpr.evaluate(), 5);
}

// Evaluate Method Tests - Complex Expression
TEST_F(JsonExpressionTest, EvaluateComplexExpression) {
    JsonExpression complexExpr(obj, "(2 + 3) * 2");
    EXPECT_EQ(complexExpr.evaluate(), 10);
}
