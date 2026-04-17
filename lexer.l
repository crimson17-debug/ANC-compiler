%{
#include "ast.h"
#include "parser.tab.h"
#include <cstdlib>
#include <cstring>
%}
%option noyywrap
%option yylineno
%%
[ \t\n\r]+              { }
"//".* { }
"out"                   { return DISPLAY; }
"in"                    { return INPUT; }
"="                     { return ASSIGN; } 
"+"|"plus"              { return PLUS; }
"-"|"minus"             { return MINUS; }
"*"|"multiply"          { return MULTIPLY; }
"/"|"divide"            { return DIVIDE; }
"<"                     { return LESS; }
">"                     { return GREATER; }
"=="                    { return EQUALS; }
"if"                    { return IF; }
"then"                  { return THEN; }
"endif"                 { return ENDIF; }
"while"                 { return WHILE; }
"do"                    { return DO; }
"endwhile"              { return ENDWHILE; }
"for"                   { return FOR; }
"from"                  { return FROM; }
"to"                    { return TO; }
"endfor"                { return ENDFOR; }
"brk"                   { return BRK; }
"cnt"                   { return CNT; }
"func"                  { return FUNC; }
"endfunc"               { return ENDFUNC; }
"rtn"                   { return RTN; }
"import"                { return IMPORT; }
"class"                 { return CLASS; }
"endclass"              { return ENDCLASS; }
"new"                   { return NEW; }
"."                     { return DOT; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"["                     { return LBRACKET; }
"]"                     { return RBRACKET; }
","                     { return COMMA; }
\"[^\"]*\"              { yytext[yyleng-1] = '\0'; yylval.str = strdup(yytext + 1); return STRING; }
[0-9]+\.[0-9]+          { yylval.fval = atof(yytext); return FLOAT_LITERAL; }                        
[0-9]+                  { yylval.num = atoi(yytext); return NUMBER; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.str = strdup(yytext); return IDENTIFIER; }
.                       { printf("Unknown character: %s\n", yytext); }
%%