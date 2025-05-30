#include "symtable.hpp"
#include <iostream>
#include <cassert>

// Mock yylineno to avoid linker errors
int yylineno = 1;

void testBasicSymTableOperations() {
    std::cout << "Testing basic SymTable operations...\n";
    
    SymTable symTable;
    
    // Test adding variables
    symTable.addVar("x", ast::BuiltInType::INT, 1);
    symTable.addVar("y", ast::BuiltInType::BOOL, 2);
    
    // Test existence check
    assert(symTable.exists("x"));
    assert(symTable.exists("y"));
    assert(!symTable.exists("z"));
    
    // Test lookup
    Symbol* xSymbol = symTable.lookup("x");
    assert(xSymbol != nullptr);
    assert(xSymbol->name == "x");
    assert(xSymbol->type == ast::BuiltInType::INT);
    assert(xSymbol->offset == 0);
    assert(!xSymbol->isFunction);
    
    Symbol* ySymbol = symTable.lookup("y");
    assert(ySymbol != nullptr);
    assert(ySymbol->name == "y");
    assert(ySymbol->type == ast::BuiltInType::BOOL);
    assert(ySymbol->offset == 1);

    
    std::cout << "Basic operations test passed!\n";
}

void testFunctionDeclarations() {
    std::cout << "Testing function declarations...\n";
    
    SymTable symTable;
    
    // Test adding functions
    std::vector<ast::BuiltInType> params = {ast::BuiltInType::INT, ast::BuiltInType::BOOL};
    symTable.addFunc("myFunc", ast::BuiltInType::STRING, 10, params);
    
    assert(symTable.exists("myFunc"));
    Symbol* funcSymbol = symTable.lookup("myFunc");
    assert(funcSymbol != nullptr);
    assert(funcSymbol->isFunction);
    assert(funcSymbol->type == ast::BuiltInType::STRING);
    assert(funcSymbol->paramTypes.size() == 2);
    assert(funcSymbol->paramTypes[0] == ast::BuiltInType::INT);
    assert(funcSymbol->paramTypes[1] == ast::BuiltInType::BOOL);
    
    std::cout << "Function declarations test passed!\n";
}

void testParameterHandling() {
    std::cout << "Testing parameter handling...\n";
    
    SymTable symTable;
    
    // Enter function scope
    symTable.enterScope();
    
    // Add parameters (should have negative offsets)
    symTable.addParam("param1", ast::BuiltInType::INT, 5);
    symTable.addParam("param2", ast::BuiltInType::BOOL, 6);
    
    Symbol* param1 = symTable.lookup("param1");
    Symbol* param2 = symTable.lookup("param2");
    
    assert(param1 != nullptr);
    assert(param2 != nullptr);
    assert(param1->offset == -1);
    assert(param2->offset == -2);
    
    // Add local variable (should reset to positive offset)
    symTable.addVar("localVar", ast::BuiltInType::STRING, 7);
    Symbol* localVar = symTable.lookup("localVar");
    assert(localVar != nullptr);
    assert(localVar->offset == 0);
    
    symTable.exitScope();
    
    std::cout << "Parameter handling test passed!\n";
}

void testScopeManagement() {
    std::cout << "Testing scope management...\n";
    
    SymTable symTable;
    
    // Add variable in global scope
    symTable.addVar("global", ast::BuiltInType::INT, 8);
    assert(symTable.exists("global"));
    
    // Enter new scope
    symTable.enterScope();
    symTable.addVar("local", ast::BuiltInType::BOOL, 9);
    assert(symTable.exists("local"));
    assert(symTable.exists("global")); // Should still exist
    
    // Exit scope
    symTable.exitScope();
    assert(!symTable.exists("local")); // Should be removed
    assert(symTable.exists("global"));  // Should still exist
    
    std::cout << "Scope management test passed!\n";
}

void testNoShadowing() {
    std::cout << "Testing no shadowing policy...\n";
    
    SymTable symTable;
    
    // Add variable in global scope
    symTable.addVar("x", ast::BuiltInType::INT, 11);
    assert(symTable.exists("x"));
    
    // Enter new scope and try to add same variable
    symTable.enterScope();
    symTable.addVar("x", ast::BuiltInType::BOOL, 12); // Should not add due to no shadowing
    
    // Should still be the original variable
    Symbol* xSymbol = symTable.lookup("x");
    assert(xSymbol != nullptr);
    assert(xSymbol->type == ast::BuiltInType::INT); // Original type
    
    symTable.exitScope();
    
    std::cout << "No shadowing test passed!\n";
}

void testBuiltInFunctions() {
    std::cout << "Testing built-in functions...\n";
    
    SymTable symTable;
    
    // Check that built-in functions are added in constructor
    assert(symTable.exists("print"));
    assert(symTable.exists("printi"));
    
    Symbol* printFunc = symTable.lookup("print");
    assert(printFunc != nullptr);
    assert(printFunc->isFunction);
    assert(printFunc->type == ast::BuiltInType::VOID);
    assert(printFunc->paramTypes.size() == 1);
    assert(printFunc->paramTypes[0] == ast::BuiltInType::STRING);
    
    Symbol* printiFunc = symTable.lookup("printi");
    assert(printiFunc != nullptr);
    assert(printiFunc->isFunction);
    assert(printiFunc->type == ast::BuiltInType::VOID);
    assert(printiFunc->paramTypes.size() == 1);
    assert(printiFunc->paramTypes[0] == ast::BuiltInType::INT);
    
    std::cout << "Built-in functions test passed!\n";
}

int main() {
    std::cout << "Starting SymTable tests...\n\n";
    
    try {
        testBasicSymTableOperations();
        testFunctionDeclarations();
        testParameterHandling();
        testScopeManagement();
        testNoShadowing();
        testBuiltInFunctions();
        
        std::cout << "\nAll tests passed successfully!\n";
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << "Test failed with unknown exception!" << std::endl;
        return 1;
    }
    
    return 0;
}
