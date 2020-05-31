%{
#include <string>
#include <iostream>

#include "nhll/nhll_driver.hpp"
#include "nhll/nhll_scanner.hpp"

#undef  YY_DECL
#define YY_DECL int NHLL::NHLL_Scanner::yylex( NHLL::NHLL_Parser::semantic_type * const lval, NHLL::NHLL_Parser::location_type *loc )

using token = NHLL::NHLL_Parser::token;

#define yyterminate() return( token::END )

#define YY_NO_UNISTD_H

#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="NHLL::NHLL_Scanner"
%option noyywrap
%option c++

%%

%{
            yylval = lval;
%}

"let"       { return( token::LET   ); }
"check"     { return( token::CHECK  ); }

"def"       { return( token::FUNC_DECL); }
"->"        { return( token::RET_ARROW); }
"int"       { return( token::INT      ); }
"real"      { return( token::REAL     ); }
"nil"       { return( token::NIL      ); }
"str"       { return( token::STR      ); }

"while"     { return( token::WHILE ); }
"loop"      { return( token::LOOP  ); }
"break"     { return( token::BREAK ); }
"global"    { return( token::GLOBAL); }

"<="        { return( token::LTE  ); }
">="        { return( token::GTE  ); }
">"         { return( token::GT   ); }
"<"         { return( token::LT   ); }
"!="        { return( token::NE   ); }
"=="        { return( token::EQ   ); }
"||"        { return( token::COR  ); }
"&&"        { return( token::CAND ); }

"lsh"       { return( token::LEFT_SH);  }
"rsh"       { return( token::RIGHT_SH); }
"or"        { return( token::OR);       }
"xor"       { return( token::XOR);      }
"and"       { return( token::AND);      }
"not"       { return( token::NOT);      }

"return"    { return( token::RETURN ); }
"yield"     { return( token::YIELD  ); }
"exit"      { return( token::EXIT   ); }

"/*"(.|\n)*?"*/" {}

"<asm>"(.|\n)*"</asm>" { 
                     yylval->build< std::string >( yytext );
                     return( token::ASM ); 
                   }

[ \t]+      { /* Eat WS */  }

[\n]+       { loc->lines(); }

"//"[^\n]*  { /* comment */ }

"#"[^\n]*   { /* preproc token */ }

\"[^\"]*\"  {
               yylval->build< std::string >( yytext );
               return( token::STRING_LITERAL );
            }

[a-zA-Z_]+  { 
               yylval->build< std::string >( yytext );
               return( token::IDENTIFIER ); 
            }
-?[0-9]+\.[0-9]+ {
               yylval->build< std::string >( yytext );
               return ( token::REAL_LITERAL );
            }

[0-9]+      {
               yylval->build< std::string >( yytext );
               return ( token::INTEGER_LITERAL );
            }


.           { /* Allow chars to slip in  */ return yytext[0]; } 


%%


