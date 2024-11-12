CXX = g++
CXXFLAGS = -std=c++17 -I ~/googletest/googletest/include
LDFLAGS = -L ~/googletest/build/lib -lgtest -lgtest_main -pthread

# Object files for the main program (excluding the main.cpp)
OBJ = json_parser.o json_expression.o json_visitor.o json_printer.o
# Object files for the tests
TEST_OBJ = json_parser_test.o

# Target for the final program (including main)
program: $(OBJ) main.o
	$(CXX) $(CXXFLAGS) -o program $^

# Target for the tests (link test objects with implementation objects)
test: $(TEST_OBJ) $(OBJ)
	$(CXX) $(CXXFLAGS) -o test_program $^ $(LDFLAGS)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f program test_program $(OBJ) $(TEST_OBJ) main.o

.PHONY: clean test
