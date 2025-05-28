#include "output.hpp"
#include "nodes.hpp"
#include "semanticvisitor.hpp"

// Extern from the bison-generated parser
extern int yyparse();

extern std::shared_ptr<ast::Node> program;

int main() {
    // Parse the input. The result is stored in the global variable `program`
    yyparse();

    // run semantic analysis
    SemanticVisitor semanticVisitor;
    program->accept(semanticVisitor);
}
