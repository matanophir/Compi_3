#ifndef SEMANTICVISITOR_HPP
#define SEMANTICVISITOR_HPP

#include "visitor.hpp"
#include "nodes.hpp"
#include "symtable.hpp"

class SemanticVisitor : public Visitor
{
private:
    SymTable symTable;
    ast::BuiltInType curr_expected_return_type;
    bool in_while;

    bool _is_numeric(ast::BuiltInType type);
    bool _can_assign(ast::BuiltInType from, ast::BuiltInType to);
    
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

    virtual void visit(ast::Block &node) override;

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

#endif //SEMANTICVISITOR_HPP
