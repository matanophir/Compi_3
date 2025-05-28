#include "semanticvisitor.hpp"

SemanticVisitor::SemanticVisitor() {
    // Constructor - symbol table is automatically initialized
}

void SemanticVisitor::visit(ast::Num &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::NumB &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::String &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Bool &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::ID &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::BinOp &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::RelOp &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Not &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::And &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Or &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::ArrayType &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::PrimitiveType &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::ArrayDereference &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::ArrayAssign &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Cast &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::ExpList &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Call &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Statements &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Break &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Continue &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Return &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::If &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::While &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::VarDecl &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Assign &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Formal &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::Formals &node) {
    // TODO: Implement
}

void SemanticVisitor::visit(ast::FuncDecl &node) {
    // TODO: Implement

}

void SemanticVisitor::visit(ast::Funcs &node) {
    // TODO: Implement
    std::vector<ast::BuiltInType> paramTypes;

    for (auto &func : node.funcs)
    {
        std::vector<ast::BuiltInType> paramTypes;
        for (const auto &formal : func->formals->formals)
        {
            paramTypes.push_back(formal->type->type);
        }
        symTable.addFunc(func->id->value, func->return_type->type, paramTypes);
    }
}
