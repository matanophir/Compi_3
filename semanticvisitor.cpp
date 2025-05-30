#include "semanticvisitor.hpp"
#include <iostream>

SemanticVisitor::SemanticVisitor() : curr_expected_return_type(ast::BuiltInType::UNDEF), in_while(false) {
    // Constructor - symbol table is automatically initialized
}

bool SemanticVisitor::_is_numeric(ast::BuiltInType type){
    return (type == ast::BuiltInType::INT || type == ast::BuiltInType::BYTE);
}

bool SemanticVisitor::_can_assign(ast::BuiltInType from, ast::BuiltInType to) {
    // Check if the assignment is valid
    if (from == to) return true; 
    if (from == ast::BuiltInType::BYTE && to == ast::BuiltInType::INT) return true;
    return false; 
}
void SemanticVisitor::visit(ast::Num &node) {
    node.computedType = ast::BuiltInType::INT;
}

void SemanticVisitor::visit(ast::NumB &node) {
    node.computedType = ast::BuiltInType::BYTE;

    // Check if the value is within the byte range
    int value = std::stoi(std::to_string(node.value), nullptr, 2);
    if (value > 255)
        output::errorByteTooLarge(node.line, value);
}

void SemanticVisitor::visit(ast::String &node) {
    node.computedType = ast::BuiltInType::STRING;
}

void SemanticVisitor::visit(ast::Bool &node) {
    node.computedType = ast::BuiltInType::BOOL;
}

void SemanticVisitor::visit(ast::ID &node) {
    Symbol* symbol = symTable.lookup(node.value);
    if (!symbol) {
        output::errorUndef(node.line, node.value);
    }
    node.computedType = symbol->type;
    node.computedIsArray = symbol->isArray;
}

void SemanticVisitor::visit(ast::BinOp &node) {
    node.left->accept(*this);
    node.right->accept(*this);


    if (!(_is_numeric(node.left->computedType) && _is_numeric(node.right->computedType))) {
        output::errorMismatch(node.line);
    }
    
    if (node.left->computedType == ast::BuiltInType::BYTE &&
        node.right->computedType == ast::BuiltInType::BYTE) {
        node.computedType = ast::BuiltInType::BYTE;
    } else {
        node.computedType = ast::BuiltInType::INT;
    }
}

void SemanticVisitor::visit(ast::RelOp &node) {
    node.left->accept(*this);
    node.right->accept(*this);

    if (!(_is_numeric(node.left->computedType) && _is_numeric(node.right->computedType))) {
        output::errorMismatch(node.line);
    }
    
    node.computedType = ast::BuiltInType::BOOL;
}

void SemanticVisitor::visit(ast::Not &node) {
    node.exp->accept(*this);

    if (node.exp->computedType != ast::BuiltInType::BOOL) {
        output::errorMismatch(node.line);
    }
    node.computedType = ast::BuiltInType::BOOL;
}

void SemanticVisitor::visit(ast::And &node) {
    node.left->accept(*this);
    node.right->accept(*this);

    if (node.left->computedType != ast::BuiltInType::BOOL || node.right->computedType != ast::BuiltInType::BOOL) {
        output::errorMismatch(node.line);
    }

    node.computedType = ast::BuiltInType::BOOL;
}

void SemanticVisitor::visit(ast::Or &node) {
    node.left->accept(*this);
    node.right->accept(*this);

    if (node.left->computedType != ast::BuiltInType::BOOL || node.right->computedType != ast::BuiltInType::BOOL) {
        output::errorMismatch(node.line);
    }

    node.computedType = ast::BuiltInType::BOOL;
}

void SemanticVisitor::visit(ast::ArrayType &node) {
    node.computedType = node.type;
    node.computedIsArray = true;
}

void SemanticVisitor::visit(ast::PrimitiveType &node) {
    node.computedType = node.type;
}

void SemanticVisitor::visit(ast::ArrayDereference &node) {
    node.id->accept(*this);
    node.index->accept(*this);

    if (!_is_numeric(node.index->computedType)) {
        output::errorMismatch(node.line);
    }

    node.computedType = node.id->computedType;
    node.computedIsArray = true;

}

void SemanticVisitor::visit(ast::Assign &node) {
    node.exp->accept(*this);
    node.id->accept(*this);

    Symbol* symbol = symTable.lookup(node.id->value); // was found in the symbol table

    if (!_can_assign(node.exp->computedType, symbol->type)) 
        output::errorMismatch(node.line);

    if (symbol->isArray) 
        output::ErrorInvalidAssignArray(node.id->line, node.id->value);

    if (node.exp->computedIsArray)
        output::errorMismatch(node.line);

}

void SemanticVisitor::visit(ast::ArrayAssign &node) {
    node.id->accept(*this);
    node.index->accept(*this);
    node.exp->accept(*this);

    Symbol* symbol = symTable.lookup(node.id->value); // was found in the symbol table

    if (!_can_assign(node.exp->computedType, symbol->type)) 
    {
        output::errorMismatch(node.line);
    }

    if (!_is_numeric(node.index->computedType)) 
    {
        output::errorMismatch(node.line);
    }
}

void SemanticVisitor::visit(ast::Cast &node) {
    node.exp->accept(*this);
    node.target_type->accept(*this);

    if (node.exp->computedType == node.target_type->computedType) {
        // OK
    }else {
        if (!(_is_numeric(node.exp->computedType) && _is_numeric(node.exp->computedType))){
            output::errorMismatch(node.line);
        }
    }

    node.computedType = node.target_type->computedType;

}

void SemanticVisitor::visit(ast::ExpList &node) {
    for (auto& exp : node.exps)
    {
        exp->accept(*this);
    }
}

void SemanticVisitor::visit(ast::Call &node) {
    node.func_id->accept(*this);
    node.args->accept(*this);

    Symbol* symbol = symTable.lookup(node.func_id->value);

    if (!symbol->isFunction)
        output::errorDefAsVar(node.func_id->line, node.func_id->value);
    
    std::vector<std::string> param_types_str = symbol->types_as_string();

    if (symbol->paramTypes.size() != node.args->exps.size()) {
        output::errorPrototypeMismatch(node.func_id->line, node.func_id->value, param_types_str);
    } else {
        for (size_t i = 0; i < node.args->exps.size(); ++i) {
            if (!_can_assign(node.args->exps[i]->computedType, symbol->paramTypes[i]) || node.args->exps[i]->computedIsArray) {
                output::errorPrototypeMismatch(node.func_id->line, node.func_id->value, param_types_str);
            }
        }
    }

    node.computedType = symbol->type;

}

void SemanticVisitor::visit(ast::Statements &node) {

    for (auto& statement : node.statements)
    {
        statement->accept(*this);
    }

}

void SemanticVisitor::visit(ast::Break &node) {
    if (!in_while) {
        output::errorUnexpectedBreak(node.line);
    }
}

void SemanticVisitor::visit(ast::Continue &node) {
    if (!in_while) {
        output::errorUnexpectedContinue(node.line);
    }
}

void SemanticVisitor::visit(ast::Return &node) {
    // Check if we're inside a function (not in global scope)
    if (curr_expected_return_type == ast::BuiltInType::UNDEF) {
        // Return statement outside of function - this is an error
        output::errorMismatch(node.line);
        return;
    }
    
    if (node.exp) {
        // Return with expression
        node.exp->accept(*this);
        ast::BuiltInType actualReturnType = node.exp->computedType;

        // cant be in a void function
        if (curr_expected_return_type == ast::BuiltInType::VOID) {
            output::errorMismatch(node.line);
        }

        //cant return an array
        if (node.exp->computedIsArray) {
            output::errorMismatch(node.line);
        }
        
        // Check if the return type matches the function's expected return type
        if (!_can_assign(actualReturnType, curr_expected_return_type)) {
            output::errorMismatch(node.line);
        }
    } else {
        // Return without expression (void return)
        if (curr_expected_return_type != ast::BuiltInType::VOID) {
            output::errorMismatch(node.line);
        }
    }
    
}

void SemanticVisitor::visit(ast::If &node) {
    symTable.enterScope();
    node.condition->accept(*this);
    if (node.condition->computedType != ast::BuiltInType::BOOL) {
        output::errorMismatch(node.condition->line);
    }

    symTable.enterScope();
    node.then->accept(*this);
    symTable.exitScope();
    symTable.exitScope();
    if (node.otherwise) {
        symTable.enterScope();
        node.otherwise->accept(*this);
        symTable.exitScope();
    }
    // symTable.exitScope();
}

void SemanticVisitor::visit(ast::While &node) {
    symTable.enterScope();
    in_while = true; // Set the flag to indicate we're in a while loop

    node.condition->accept(*this);
    if (node.condition->computedType != ast::BuiltInType::BOOL) {
        output::errorMismatch(node.condition->line);
    }

    symTable.enterScope();

    node.body->accept(*this);

    symTable.exitScope();
    in_while = false; // Reset the flag after exiting the while loop
    symTable.exitScope();
}

void SemanticVisitor::visit(ast::VarDecl &node) {
    node.type->accept(*this);

    symTable.addVar(node.id->value, node.type->computedType, node.id->line, node.type->computedIsArray);

    node.id->accept(*this);


    if (node.init_exp) {
        node.init_exp->accept(*this);
        // If there is an initial value, check if it matches the type
        if (!_can_assign(node.init_exp->computedType, node.type->computedType)) {
            output::errorMismatch(node.line);
        }
    } else {
        // If there is no initial value, ensure the type is not void
        if (node.type->computedType == ast::BuiltInType::VOID) {
            output::errorMismatch(node.line);
        }
    }
}

void SemanticVisitor::visit(ast::Formal &node) {
    node.type->accept(*this);
    
    symTable.addParam(node.id->value, node.type->computedType, node.id->line);
}

void SemanticVisitor::visit(ast::Formals &node) {
    for (auto& formal : node.formals)
    {
        formal->accept(*this);
    }
}

void SemanticVisitor::visit(ast::FuncDecl &node) {
    node.id->accept(*this);
    node.return_type->accept(*this);


    // Save the previous expected return type and set the new one
    ast::BuiltInType prev_expected_return_type = curr_expected_return_type;
    curr_expected_return_type = node.return_type->computedType;
    
    symTable.enterScope();

    // accepting the formals to add them to the symbol table
    node.formals->accept(*this);

    node.body->accept(*this);

    symTable.exitScope();
    
    // Restore the previous expected return type
    curr_expected_return_type = prev_expected_return_type;
}

void SemanticVisitor::visit(ast::Funcs &node) {
    // first adding all functions to the symbol table
    for (auto &func : node.funcs)
    {
        std::vector<ast::BuiltInType> paramTypes;

        for (const auto &formal : func->formals->formals)
        {
            formal->type->accept(*this);
            paramTypes.push_back(formal->type->computedType);
        }
        symTable.addFunc(func->id->value, func->return_type->computedType, func->id->line, paramTypes);
    }

    //check if there one and only one main function
    bool has_main = false;
    for (const auto &func : node.funcs)
    {
        if (func->id->value == "main" && func->return_type->computedType == ast::BuiltInType::VOID &&
            func->formals->formals.empty())
        {
            if (has_main) {
                output::errorMainMissing();
            }
            has_main = true;
        }
    }

    // then visiting each function to process its body
    for (auto &func : node.funcs)
    {
        func->accept(*this);
    }

}
