#ifndef SYMTABLE_HPP
#define SYMTABLE_HPP

#include <vector>
#include <string>
#include <stack>
#include <unordered_map>
#include "nodes.hpp"
#include "output.hpp"

// Table entry structure to hold symbol information
struct Symbol {
    std::string name;
    ast::BuiltInType type;
    int offset;
    int lineno; 
    bool isFunction;
    bool isArray;
    std::vector<ast::BuiltInType> paramTypes; // For functions

    Symbol() = default;
    
    Symbol(const std::string& n, ast::BuiltInType t, int lineno, int o, bool isFunc = false, bool isArr = false) 
        : name(n), type(t), offset(o), lineno(lineno), isFunction(isFunc), isArray(isArr) {}
    
    std::vector<std::string> types_as_string();
};

// Type alias for table
using Table = std::vector<Symbol>;

// Scope class to hold a table
class Scope {
public:
    Table table;

    Scope() = default;
};

class SymTable
{
private:
    // Stack of scopes - each scope contains a table and while loop state
    std::stack<Scope> scopesStack;
    
    // Stack of offsets for each scope
    std::stack<int> offsetsStack;
    
    // Global map to check if a symbol exists (no shadowing allowed)
    std::unordered_map<std::string, Symbol> symbols;
    
    // ScopePrinter for output
    output::ScopePrinter scopePrinter;

    void _check_before_add(const std::string& name);

public:
    SymTable();
    ~SymTable() { printScopes(); }
    
    // Scope management
    void enterScope();
    void exitScope();
    Scope& getCurrentScope();
    
    // Symbol management
    void addVar(const std::string& name, ast::BuiltInType type, int lineno, bool isArray = false);
    void addFunc(const std::string& name, ast::BuiltInType returnType, int lineno,
                 const std::vector<ast::BuiltInType>& paramTypes);
    void addParam(const std::string& name, ast::BuiltInType type, int lineno);
    
    // Symbol lookup
    bool exists(const std::string& name) const;
    Symbol* lookup(const std::string& name);
    
    // Print current state (handled internally by ScopePrinter)
    void printScopes() const;
};

#endif //SYMTABLE_HPP
