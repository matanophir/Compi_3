#ifndef OUTPUT_HPP
#define OUTPUT_HPP

#include <vector>
#include <string>
#include <sstream>
#include <stack>
#include <unordered_map>
#include "visitor.hpp"
#include "nodes.hpp"
#include "symtable.hpp"

namespace output {
    /* Error handling functions */

    void errorLex(int lineno);

    void errorSyn(int lineno);

    void errorUndef(int lineno, const std::string &id);

    void errorDefAsFunc(int lineno, const std::string &id);

    void errorUndefFunc(int lineno, const std::string &id);

    void errorDefAsVar(int lineno, const std::string &id);

    void errorDef(int lineno, const std::string &id);

    void errorPrototypeMismatch(int lineno, const std::string &id, std::vector<std::string> &paramTypes);

    void errorMismatch(int lineno);

    void errorUnexpectedBreak(int lineno);

    void errorUnexpectedContinue(int lineno);

    void errorMainMissing();

    void errorByteTooLarge(int lineno, int value);

    void ErrorInvalidAssignArray(int lineno, const std::string &id_arr);

    /* ScopePrinter class
     * This class is used to print scopes in a human-readable format.
     */
    class ScopePrinter {
    private:
        std::stringstream globalsBuffer;
        std::stringstream buffer;
        int indentLevel;

        std::string indent() const;

    public:
        ScopePrinter();

        void beginScope();

        void endScope();

        void emitVar(const std::string &id, const ast::BuiltInType &type, int offset);

        void emitFunc(const std::string &id, const ast::BuiltInType &returnType,
                      const std::vector<ast::BuiltInType> &paramTypes);

        friend std::ostream &operator<<(std::ostream &os, const ScopePrinter &printer);
    };

}

class SemanticVisitor : public Visitor
{
private:
    SymTable symbolTable;
    
public:
    SemanticVisitor();
    
    virtual void visit(ast::Num &node) override;

    virtual void visit(ast::NumB &node) override;

    virtual void visit(ast::String &node) override;

    virtual void visit(ast::Bool &node) override;

    virtual void visit(ast::ID &node) override;

    virtual void visit(ast::BinOp &node) override;

    virtual void visit(ast::RelOp &node) override;

    virtual void visit(ast::Not &node) override;

    virtual void visit(ast::And &node) override;

    virtual void visit(ast::Or &node) override;

    // virtual void visit(ast::Type &node) override;

    virtual void visit(ast::ArrayType &node) override;

    virtual void visit(ast::PrimitiveType &node) override;

    virtual void visit(ast::ArrayDereference &node) override;

    virtual void visit(ast::ArrayAssign &node) override;

    virtual void visit(ast::Cast &node) override;

    virtual void visit(ast::ExpList &node) override;

    virtual void visit(ast::Call &node) override;

    virtual void visit(ast::Statements &node) override;

    virtual void visit(ast::Break &node) override;

    virtual void visit(ast::Continue &node) override;

    virtual void visit(ast::Return &node) override;

    virtual void visit(ast::If &node) override;

    virtual void visit(ast::While &node) override;

    virtual void visit(ast::VarDecl &node) override;

    virtual void visit(ast::Assign &node) override;

    virtual void visit(ast::Formal &node) override;

    virtual void visit(ast::Formals &node) override;

    virtual void visit(ast::FuncDecl &node) override;

    virtual void visit(ast::Funcs &node) override;
};
#endif //OUTPUT_HPP
