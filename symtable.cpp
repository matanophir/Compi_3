#include "symtable.hpp"
#include <iostream>


std::vector<std::string> Symbol::types_as_string() {
    std::vector<std::string> typeStrings;
    for (const auto& type : paramTypes) {
        typeStrings.push_back(output::toString(type));
    }
    return typeStrings;
}

/* SymTable class implementation */

SymTable::SymTable() {
    // Initialize with global scope
    scopesStack.push(Scope());
    int currentOffset = 0;
    offsetsStack.push(currentOffset);

    addFunc("print", ast::BuiltInType::VOID, 0, {ast::BuiltInType::STRING});
    addFunc("printi", ast::BuiltInType::VOID, 0, {ast::BuiltInType::INT});
}

void SymTable::_check_before_add(const std::string& name) {
    Symbol* existingSymbol = lookup(name);
    if (existingSymbol != nullptr)
    {
        if (existingSymbol->isFunction) {
            output::errorDefAsFunc(existingSymbol->lineno, name);
        } else {
            output::errorDefAsVar(existingSymbol->lineno, name);
        }
    }
}

void SymTable::enterScope() {
    scopesStack.push(Scope());

    int currentOffset = offsetsStack.top();
    offsetsStack.push(currentOffset);

    scopePrinter.beginScope();
}

void SymTable::exitScope() {
    // Remove symbols from global map for this scope
    Table& currentTable = scopesStack.top().table;
    for (const auto& entry : currentTable) {
        symbols.erase(entry.name);
    }
    
    scopesStack.pop();
    offsetsStack.pop();
    scopePrinter.endScope();
}

Scope& SymTable::getCurrentScope() {
    return scopesStack.top();
}

void SymTable::addVar(const std::string& name, ast::BuiltInType type, int lineno) {

    _check_before_add(name);
    
    // If offset is negative, reset to 0 for local variables
    if (offsetsStack.top() < 0) {
        offsetsStack.top() = 0;
    }
    
    int currentOffset = offsetsStack.top();
    Symbol entry(name, type, lineno, currentOffset, false);
    scopesStack.top().table.push_back(entry);
    symbols[name] = entry;
    
    scopePrinter.emitVar(name, type, currentOffset);
    // Increment offset directly
    offsetsStack.top() += 1;
}

void SymTable::addFunc(const std::string& name, ast::BuiltInType returnType, int lineno,
                       const std::vector<ast::BuiltInType>& paramTypes) {
    
    _check_before_add(name);
    
    Symbol entry(name, returnType, lineno, 0, true);
    entry.paramTypes = paramTypes;
    scopesStack.top().table.push_back(entry);
    symbols[name] = entry;
    
    scopePrinter.emitFunc(name, returnType, paramTypes);
}

void SymTable::addParam(const std::string& name, ast::BuiltInType type, int lineno) {
    
    _check_before_add(name);
    // Decrement offset first to get negative values
    offsetsStack.top() -= 1;
    int currentOffset = offsetsStack.top();
    
    Symbol entry(name, type, lineno, currentOffset, false);
    // Insert at the beginning of the vector for reverse order
    scopesStack.top().table.insert(scopesStack.top().table.begin(), entry);
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
