#include "symtable.hpp"
#include <iostream>

/* SymTable class implementation */

SymTable::SymTable() {
    // Initialize with global scope
    enterScope();
    addFunc("print", ast::BuiltInType::VOID, {ast::BuiltInType::STRING});
    addFunc("printi", ast::BuiltInType::VOID, {ast::BuiltInType::INT});
}

void SymTable::enterScope() {
    tablesStack.push(Table());
    int currentOffset = offsetsStack.empty() ? 0 : offsetsStack.top();
    offsetsStack.push(currentOffset);
    scopePrinter.beginScope();
}

void SymTable::exitScope() {
    // Remove symbols from global map for this scope
    Table& currentTable = tablesStack.top();
    for (const auto& entry : currentTable) {
        symbols.erase(entry.name);
    }
    
    tablesStack.pop();
    offsetsStack.pop();
    scopePrinter.endScope();
}

void SymTable::addVar(const std::string& name, ast::BuiltInType type) {
    if (exists(name)) {
        // Symbol already exists - this is an error
        return;
    }
    
    // If offset is negative, reset to 0 for local variables
    if (offsetsStack.top() < 0) {
        offsetsStack.top() = 0;
    }
    
    int currentOffset = offsetsStack.top();
    Symbol entry(name, type, currentOffset, false);
    tablesStack.top().push_back(entry);
    symbols[name] = entry;
    
    scopePrinter.emitVar(name, type, currentOffset);
    // Increment offset directly
    offsetsStack.top() += 1;
}

void SymTable::addFunc(const std::string& name, ast::BuiltInType returnType, 
                       const std::vector<ast::BuiltInType>& paramTypes) {
    if (exists(name)) {
        // Symbol already exists - this is an error
        return;
    }
    
    Symbol entry(name, returnType, 0, true);
    entry.paramTypes = paramTypes;
    tablesStack.top().push_back(entry);
    symbols[name] = entry;
    
    scopePrinter.emitFunc(name, returnType, paramTypes);
}

void SymTable::addParam(const std::string& name, ast::BuiltInType type) {
    if (exists(name)) {
        // Symbol already exists - this is an error
        return;
    }
    
    // Decrement offset first to get negative values
    offsetsStack.top() -= 1;
    int currentOffset = offsetsStack.top();
    
    Symbol entry(name, type, currentOffset, false);
    // Insert at the beginning of the vector for reverse order
    tablesStack.top().insert(tablesStack.top().begin(), entry);
    symbols[name] = entry;
    
    scopePrinter.emitVar(name, type, currentOffset);
}

bool SymTable::exists(const std::string& name) const {
    return symbols.find(name) != symbols.end();
}

Symbol* SymTable::lookup(const std::string& name) {
    // First check the global map for existence
    auto globalIt = symbols.find(name);
    if (globalIt != symbols.end()) {
        // Return a pointer to the entry in the global map
        return &(globalIt->second);
    }
    
    return nullptr;
}

void SymTable::printScopes() const {
    std::cout << scopePrinter << std::endl;
}
