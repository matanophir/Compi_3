#ifndef SYMTABLE_HPP
#define SYMTABLE_HPP

#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include "nodes.hpp"
#include "output.hpp"

// Table entry structure to hold symbol information
struct TableEntry {
    std::string name;
    ast::BuiltInType type;
    int offset;
    bool isFunction;
    std::vector<ast::BuiltInType> paramTypes; // For functions

    TableEntry() = default;
    
    TableEntry(const std::string& n, ast::BuiltInType t, int o, bool isFunc = false) 
        : name(n), type(t), offset(o), isFunction(isFunc) {}
};

// Type alias for table
using Table = std::vector<TableEntry>;

class SymTable
{
private:
    // Stack of tables - each table is a vector of TableEntry
    std::stack<Table> tablesStack;
    
    // Stack of offsets for each scope
    std::stack<int> offsetsStack;
    
    // Global map to check if a symbol exists (no shadowing allowed)
    std::unordered_map<std::string, TableEntry> symbols;
    
    // ScopePrinter for output
    output::ScopePrinter scopePrinter;

public:
    SymTable();
    ~SymTable() { printScopes(); }
    
    // Scope management
    void enterScope();
    void exitScope();
    
    // Symbol management
    void addVar(const std::string& name, ast::BuiltInType type);
    void addFunc(const std::string& name, ast::BuiltInType returnType, 
                 const std::vector<ast::BuiltInType>& paramTypes);
    void addParam(const std::string& name, ast::BuiltInType type);
    
    // Symbol lookup
    bool exists(const std::string& name) const;
    TableEntry* lookup(const std::string& name);
    
    // Print current state (handled internally by ScopePrinter)
    void printScopes() const;
};

#endif //SYMTABLE_HPP
