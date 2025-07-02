%{

#include "nodes.hpp"
#include "output.hpp"
#include <iostream>

// bison declarations
extern int yylineno;
extern int yylex();

void yyerror(const char*);

// root of the AST, set by the parser and used by other parts of the compiler
std::shared_ptr<ast::Node> program;

using namespace std;

template <typename T>
std::shared_ptr<T> as(std::shared_ptr<ast::Node> node) {
    auto result = std::dynamic_pointer_cast<T>(node);
    if (!result && node) { 
        cout << "Error: Failed to cast AST node to appropriate type on line " << yylineno << endl;
    }
    return result;
}

// TODO: Place any additional declarations here
%}

// TODO: Define tokens here

%token VOID INT BYTE BOOL AND OR NOT TRUE FALSE RETURN IF ELSE WHILE BREAK CONTINUE
%token SC COMMA LPAREN RPAREN LBRACE RBRACE LBRACK RBRACK ASSIGN RELOP 
%token ID NUM NUM_B STRING

%right ASSIGN
%left OR
%left AND
%left RELOP
%left '+' '-'
%left '*' '/'
%right NOT
%nonassoc CAST
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%

// While reducing the start variable, set the root of the AST
Program  
    : Funcs                                     { program = $1; }
    ;

// TODO: Define grammar here
Funcs
    :                                           { $$ = make_shared<ast::Funcs>(); }
    | FuncDecl Funcs                            { 
                                                  $$ = $2; 
                                                  as<ast::Funcs>($$)->push_front(as<ast::FuncDecl>($1)); 
                                                }
    
    ;

FuncDecl
    : RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE
                                                { 
                                                  $$ = make_shared<ast::FuncDecl>(
                                                    as<ast::ID>($2), 
                                                    as<ast::PrimitiveType>($1), 
                                                    as<ast::Formals>($4), 
                                                    as<ast::Statements>($7)
                                                  ); 
                                                }
    ;

RetType
    : Type                                      { 
                                                  $$ = make_shared<ast::PrimitiveType>(as<ast::PrimitiveType>($1)->type); 
                                                }
    | VOID                                      { $$ = make_shared<ast::PrimitiveType>(ast::BuiltInType::VOID); }
    ;

Formals
    :                                           { $$ = make_shared<ast::Formals>(); }
    | FormalsList                               { $$ = $1;}
    ;

FormalsList
    : FormalDecl                                { 
                                                  $$ = make_shared<ast::Formals>(as<ast::Formal>($1)); 
                                                }
    | FormalDecl COMMA FormalsList              { 
                                                  $$ = $3; 
                                                  as<ast::Formals>($$)->push_front(as<ast::Formal>($1)); 
                                                }
    ;

FormalDecl
    : Type ID                                   { 
                                                  $$ = make_shared<ast::Formal>(as<ast::ID>($2), as<ast::PrimitiveType>($1)); 
                                                }
    ;

Statements
    : Statement                                 { 
                                                  $$ = make_shared<ast::Statements>(as<ast::Statement>($1)); 
                                                }
    | Statements Statement                      { 
                                                  $$ = $1; 
                                                  as<ast::Statements>($$)->push_back(as<ast::Statement>($2)); 
                                                } 
    ;

Statement 
    : LBRACE Statements RBRACE                  { $$ = make_shared<ast::Block>(as<ast::Statements>($2)); }
    | Type ID SC                                { 
                                                  $$ = make_shared<ast::VarDecl>(as<ast::ID>($2), as<ast::PrimitiveType>($1)); 
                                                }
    | Type ID ASSIGN Exp SC                     { 
                                                  $$ = make_shared<ast::VarDecl>(
                                                    as<ast::ID>($2),
                                                    as<ast::PrimitiveType>($1), 
                                                    as<ast::Exp>($4)
                                                  ); 
                                                }
    | ID ASSIGN Exp SC                          { 
                                                  $$ = make_shared<ast::Assign>(as<ast::ID>($1), as<ast::Exp>($3)); 
                                                }
    | ID LBRACK Exp RBRACK ASSIGN Exp SC        { 
                                                  $$ = make_shared<ast::ArrayAssign>(
                                                    as<ast::ID>($1), 
                                                    as<ast::Exp>($6), 
                                                    as<ast::Exp>($3)
                                                  ); 
                                                }
    | Type ID LBRACK Exp RBRACK SC              { 
                                                  $$ = make_shared<ast::VarDecl>(as<ast::ID>($2), make_shared<ast::ArrayType>(as<ast::PrimitiveType>($1)->type, as<ast::Exp>($4))); 
                                                }
    | Call SC                                   { $$ = $1; }
    | RETURN SC                                 { $$ = make_shared<ast::Return>(); }
    | RETURN Exp SC                             { 
                                                  $$ = make_shared<ast::Return>(as<ast::Exp>($2)); 
                                                }
    | IF LPAREN Exp RPAREN Statement %prec LOWER_THAN_ELSE     
                                                { 
                                                  $$ = make_shared<ast::If>(
                                                    as<ast::Exp>($3), 
                                                    as<ast::Statement>($5)
                                                  ); 
                                                }
    | IF LPAREN Exp RPAREN Statement ELSE Statement
                                                { 
                                                  $$ = make_shared<ast::If>(
                                                    as<ast::Exp>($3), 
                                                    as<ast::Statement>($5), 
                                                    as<ast::Statement>($7)
                                                  ); 
                                                }
    | WHILE LPAREN Exp RPAREN Statement         { 
                                                  $$ = make_shared<ast::While>(
                                                    as<ast::Exp>($3), 
                                                    as<ast::Statement>($5)
                                                  ); 
                                                }
    | BREAK SC                                  { $$ = make_shared<ast::Break>(); }
    | CONTINUE SC                               { $$ = make_shared<ast::Continue>(); }
    ;

Call
    : ID LPAREN ExpList RPAREN                  { 
                                                  $$ = make_shared<ast::Call>(as<ast::ID>($1), as<ast::ExpList>($3)); 
                                                }
    | ID LPAREN RPAREN                          { 
                                                  $$ = make_shared<ast::Call>(as<ast::ID>($1)); 
                                                }
    ;

ExpList
    : Exp                                      { 
                                                  $$ = make_shared<ast::ExpList>(as<ast::Exp>($1)); 
                                                }
    | Exp COMMA ExpList                        { 
                                                  $$ = $3; 
                                                  as<ast::ExpList>($$)->push_front(as<ast::Exp>($1)); 
                                                }
    ;

Type
    : INT                                       { $$ = make_shared<ast::PrimitiveType>(ast::BuiltInType::INT); }
    | BYTE                                      { $$ = make_shared<ast::PrimitiveType>(ast::BuiltInType::BYTE); }
    | BOOL                                      { $$ = make_shared<ast::PrimitiveType>(ast::BuiltInType::BOOL); }
    ;

Exp
    : LPAREN Exp RPAREN                         { $$ = $2; }
    | ID LBRACK Exp RBRACK                      { $$ = make_shared<ast::ArrayDereference>(as<ast::ID>($1), as<ast::Exp>($3)); }
    | Exp '+' Exp                               { $$ = make_shared<ast::BinOp>(as<ast::Exp>($1), as<ast::Exp>($3), ast::BinOpType::ADD); }
    | Exp '-' Exp                               { $$ = make_shared<ast::BinOp>(as<ast::Exp>($1), as<ast::Exp>($3), ast::BinOpType::SUB); }
    | Exp '*' Exp                               { $$ = make_shared<ast::BinOp>(as<ast::Exp>($1), as<ast::Exp>($3), ast::BinOpType::MUL); }
    | Exp '/' Exp                               { $$ = make_shared<ast::BinOp>(as<ast::Exp>($1), as<ast::Exp>($3), ast::BinOpType::DIV); }
    | ID                                        { $$ = $1; }
    | Call                                      { $$ = $1; }
    | NUM                                       { $$ = $1; }
    | NUM_B                                     { $$ = $1; }
    | STRING                                    { $$ = $1; }
    | TRUE                                      { $$ = make_shared<ast::Bool>(true); }
    | FALSE                                     { $$ = make_shared<ast::Bool>(false); }
    | NOT Exp                                   { $$ = make_shared<ast::Not>(as<ast::Exp>($2)); }
    | Exp AND Exp                               { $$ = make_shared<ast::And>(as<ast::Exp>($1), as<ast::Exp>($3)); }
    | Exp OR Exp                                { $$ = make_shared<ast::Or>(as<ast::Exp>($1), as<ast::Exp>($3)); }
    | Exp RELOP Exp                             { 
                                                  $$ = make_shared<ast::RelOp>(
                                                    as<ast::Exp>($1), 
                                                    as<ast::Exp>($3), 
                                                    as<ast::RelOp>($2)->op
                                                  ); 
                                                }
    | LPAREN Type RPAREN Exp %prec CAST
                                                { 
                                                  $$ = make_shared<ast::Cast>(
                                                    as<ast::Exp>($4), 
                                                    as<ast::PrimitiveType>($2)
                                                  ); 
                                                }
    ;


%%

// TODO: Place any additional code here

void yyerror(const char* /*msg*/) {
    output::errorSyn(yylineno);
    exit(0);
}