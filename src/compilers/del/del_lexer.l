%{
#include <string>
#include <iostream>

#include "del/ast/Types.hpp"
#include "del/del_driver.hpp"
#include "del/del_scanner.hpp"

#undef  YY_DECL
#define YY_DECL int DEL::DEL_Scanner::yylex( DEL::DEL_Parser::semantic_type * const lval, DEL::DEL_Parser::location_type *loc )

using token = DEL::DEL_Parser::token;

#define yyterminate() return( token::END )

#define YY_NO_UNISTD_H

#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

%}

%option debug
%option nodefault
%option yyclass="DEL::DEL_Scanner"
%option noyywrap
%option yylineno
%option c++

%%

%{
   /*
         Note: The tokens listed here for operations must be reflected
         in Intermediate.cpp as to ensure proper byte generation
   */
            yylval = lval;
%}

"int"       { return token::INT;    }
"real"      { return token::REAL;   }
"char"      { return token::CHAR;   }
"nil"       { return token::NIL;    }
"->"        { return token::ARROW;  }
"return"    { return token::RETURN; }
"if"        { return token::IF;     }
"elif"      { return token::ELIF;   }
"else"      { 
               yylval->build< int >( yylineno );
               return ( token::ELSE );    
            }

"<="       { return token::LTE;    }
"<"        { return token::LT;     }
">="       { return token::GTE;    }
">"        { return token::GT;     }
"=="       { return token::EQ;     }
"!="       { return token::NE;     }
"<<"       { return token::LSH;    }
">>"       { return token::RSH;    }
"!"        { return token::NEGATE; }
"||"       { return token::OR;     }
"&&"       { return token::AND;    }
"or"       { return token::BW_OR;  }
"xor"      { return token::BW_XOR; }
"and"      { return token::BW_AND; }
"not"      { return token::BW_NOT; }

"/"        { return token::DIV; }
"+"        { return token::ADD; }
"-"        { return token::SUB; }
"*"        { return token::MUL; }
"^"        { return token::POW; }
"%"        { return token::MOD; }

"="        { return token::ASSIGN; }
"."        { return token::DOT;    }
","        { return token::COMMA;  }

"("        { return token::LEFT_PAREN; }
"{"        { return token::LEFT_BRACKET;    }
")"        { 
               yylval->build< int >( yylineno );
               return ( token::RIGHT_PAREN );    
           }
"}"        { 
               yylval->build< int >( yylineno );
               return ( token::RIGHT_BRACKET );    
           }
"def"      {
               yylval->build< int >( yylineno );
               return ( token::DEF );
           }
";"        {
               yylval->build< int >( yylineno );
               return ( token::SEMI );
           }

(@file.*)  { driver.preproc_file_directive(yytext); }

\/\/.*     { /* ignore comments */ }

[ \t]+      { /* Eat WS */  }

[\n]+       { loc->lines(); }

[a-zA-Z_]+  { 
               yylval->build< std::string >( yytext );
               return( token::IDENTIFIER ); 
            }

[0-9]+\.[0-9]+ {
               yylval->build< std::string >( yytext );
               return ( token::REAL_LITERAL );
            }

[0-9]+      {
               yylval->build< std::string >( yytext );
               return ( token::INT_LITERAL );
            }

0[xX][0-9a-fA-F]+ {
               yylval->build< std::string >( yytext );
               return ( token::HEX_LITERAL );
            }

\".\"  {
               yylval->build< std::string >( yytext );
               return( token::CHAR_LITERAL );
            }

.           { /* Allow chars to slip in  */ return yytext[0]; } 


%%


