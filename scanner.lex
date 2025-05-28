%{
#include "output.hpp"
#include "nodes.hpp"
#include "parser.tab.hpp"
#include <string>
#include <memory>

#include <iostream>
using namespace std;
%}

%option noyywrap
%option yylineno


digit        [0-9]
letter       [a-zA-Z]
id           {letter}({letter}|{digit})*
num          0|([1-9]{digit}*)
num_b        {num}b
whitespace   [ \t\r\n]
comment      \/\/[^\r\n]*[\r|\n|\r\n]?
string \"([^\n\r\"\\]|\\[rnt\"\\])+\"

%%

{comment}                      { /* ignore comments */ }
{whitespace}                   { /* skip */ }

"void"                         { return VOID; }
"int"                          { return INT; }
"byte"                         { return BYTE; }
"bool"                         { return BOOL; }
"and"                          { return AND; }
"or"                           { return OR; }
"not"                          { return NOT; }
"true"                         { return TRUE; }
"false"                        { return FALSE; }
"return"                       { return RETURN; }
"if"                           { return IF; }
"else"                         { return ELSE; }
"while"                        { return WHILE; }
"break"                        { return BREAK; }
"continue"                     { return CONTINUE; }

";"                            { return SC; }
","                            { return COMMA; }
"("                            { return LPAREN; }
")"                            { return RPAREN; }
"{"                            { return LBRACE; }
"}"                            { return RBRACE; }
"["                            { return LBRACK; }
"]"                            { return RBRACK; }
"="                            { return ASSIGN; }

"=="                           { yylval = make_shared<ast::RelOp>(nullptr, nullptr, ast::RelOpType::EQ); return RELOP; }
"!="                           { yylval = make_shared<ast::RelOp>(nullptr, nullptr, ast::RelOpType::NE); return RELOP; }
"<="                           { yylval = make_shared<ast::RelOp>(nullptr, nullptr, ast::RelOpType::LE); return RELOP; }
">="                           { yylval = make_shared<ast::RelOp>(nullptr, nullptr, ast::RelOpType::GE); return RELOP; }
"<"                            { yylval = make_shared<ast::RelOp>(nullptr, nullptr, ast::RelOpType::LT); return RELOP; }
">"                            { yylval = make_shared<ast::RelOp>(nullptr, nullptr, ast::RelOpType::GT); return RELOP; }

"+"|"-"|"*"|"/"                { return yytext[0]; }

{id}                           {
                                 yylval = make_shared<ast::ID>(yytext);
                                 return ID;
                               }

{num}                          {
                                 yylval = make_shared<ast::Num>(yytext);
                                 return NUM;
                               }

{num_b}                        {
                                 yylval = make_shared<ast::NumB>(yytext);
                                 return NUM_B;
                               }


{string}                       {
                                 yylval = make_shared<ast::String>(yytext);
                                 return STRING;
                               }


.                              {
                                 output::errorLex(yylineno);
                                 exit(0);
                               }

%%
