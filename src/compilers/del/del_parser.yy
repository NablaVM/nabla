%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {DEL}

%define api.parser.class {DEL_Parser}

%define parse.lac full
%define parse.error verbose

%code requires{
   
   namespace DEL 
   {
      class DEL_Driver;
      class DEL_Scanner;
      class Node;
      class Assignment;
      class AST;
      class Element;
      class Function;
      struct FunctionParam;
   }

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { DEL_Scanner  &scanner  }
%parse-param { DEL_Driver  &driver  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   #include <stdint.h>
   #include <vector>

   #include "Ast.hpp"
   #include "Types.hpp"
   
   #include "del_driver.hpp"

   std::vector<DEL::FunctionParam> r_params;
   std::vector<DEL::FunctionParam> c_params;

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%token INT REAL CHAR ARROW RETURN LTE GTE GT LT EQ NE BW_NOT DIV ADD SUB MUL POW MOD
%token LSH RSH BW_OR BW_AND BW_XOR AND OR NEGATE NIL
%token LEFT_PAREN LEFT_BRACKET ASSIGN DOT COMMA IF ELIF

%type<DEL::Element*> stmt;
%type<DEL::Element*> assignment;
%type<DEL::Element*> reassignment;
%type<DEL::Element*> return_stmt;
%type<DEL::Element*> if_stmt;
%type<DEL::Element*> elif_stmt;
%type<DEL::Element*> else_stmt;
%type<DEL::Element*> direct_function_call;
%type<DEL::Function*> function_stmt;
%type<std::string> identifiers;
%type<DEL::FunctionParam*> call_item;

%type<DEL::AST*> expr_function_call;
%type<DEL::AST*> expression;
%type<DEL::AST*> term;
%type<DEL::AST*> factor;
%type<DEL::AST*> primary;
%type<DEL::AST*> primary_char;
%type<int> value_types;

%type<std::vector<DEL::Element*>> multiple_statements;
%type<std::vector<DEL::Element*>> block;

%token <std::string> INT_LITERAL
%token <std::string> HEX_LITERAL
%token <std::string> REAL_LITERAL
%token <std::string> CHAR_LITERAL
%token <std::string> IDENTIFIER
%token <int>         RIGHT_BRACKET  // These tokens encode line numbers
%token <int>         RIGHT_PAREN    // These tokens encode line numbers
%token <int>         SEMI           // These tokens encode line numbers
%token <int>         DEF            // These tokens encode line numbers
%token <int>         ELSE           // These tokens encode line numbers
%token               END    0     "end of file"
%locations
%start start

%%

start 
   : END                   { driver.indicate_complete(); }
   | input END             { driver.indicate_complete(); }
   ; 

input
   : function_stmt         { driver.build_function($1); }
   | input function_stmt   { driver.build_function($2); }
   ;

expression
   : term                        { $$ = $1;  }
   | expression ADD term         { $$ = new DEL::AST(DEL::NodeType::ADD, $1, $3);  }
   | expression SUB term         { $$ = new DEL::AST(DEL::NodeType::SUB, $1, $3);  }
   | expression LTE term         { $$ = new DEL::AST(DEL::NodeType::LTE, $1, $3);  }
   | expression GTE term         { $$ = new DEL::AST(DEL::NodeType::GTE, $1, $3);  }
   | expression GT  term         { $$ = new DEL::AST(DEL::NodeType::GT , $1, $3);  }
   | expression LT  term         { $$ = new DEL::AST(DEL::NodeType::LT , $1, $3);  }
   | expression EQ  term         { $$ = new DEL::AST(DEL::NodeType::EQ , $1, $3);  }
   | expression NE  term         { $$ = new DEL::AST(DEL::NodeType::NE , $1, $3);  }
   ;

term
   : factor                      { $$ = $1;  }
   | term MUL factor             { $$ = new DEL::AST(DEL::NodeType::MUL,    $1, $3);  }
   | term DIV factor             { $$ = new DEL::AST(DEL::NodeType::DIV,    $1, $3);  }
   | term POW factor             { $$ = new DEL::AST(DEL::NodeType::POW,    $1, $3);  }
   | term MOD factor             { $$ = new DEL::AST(DEL::NodeType::MOD,    $1, $3);  }
   | term LSH factor             { $$ = new DEL::AST(DEL::NodeType::LSH,    $1, $3);  }
   | term RSH factor             { $$ = new DEL::AST(DEL::NodeType::RSH,    $1, $3);  }
   | term BW_XOR factor          { $$ = new DEL::AST(DEL::NodeType::BW_XOR, $1, $3);  }
   | term BW_OR factor           { $$ = new DEL::AST(DEL::NodeType::BW_OR,  $1, $3);  }
   | term BW_AND factor          { $$ = new DEL::AST(DEL::NodeType::BW_AND, $1, $3);  }
   | term OR factor              { $$ = new DEL::AST(DEL::NodeType::OR,     $1, $3);  }
   | term AND factor             { $$ = new DEL::AST(DEL::NodeType::AND,    $1, $3);  }
   ;

factor
   : primary                     { $$ = $1; }
   | expr_function_call          { $$ = $1; }
   | LEFT_PAREN expression RIGHT_PAREN    { $$ = $2; }
   | BW_NOT factor               { $$ = new DEL::AST(DEL::NodeType::BW_NOT, $2, nullptr);}
   | NEGATE factor               { $$ = new DEL::AST(DEL::NodeType::NEGATE, $2, nullptr);}
   ;

expr_function_call
   : identifiers LEFT_PAREN RIGHT_PAREN             { $$ = new DEL::Call($1, c_params, nullptr, nullptr, $3); }
   | identifiers LEFT_PAREN call_params RIGHT_PAREN { $$ = new DEL::Call($1, c_params, nullptr, nullptr, $4); c_params.clear(); }
   ;

primary
    : INT_LITERAL                { $$ = new DEL::AST(DEL::NodeType::VAL, nullptr, nullptr, DEL::ValType::INTEGER, $1); }
    | REAL_LITERAL               { $$ = new DEL::AST(DEL::NodeType::VAL, nullptr, nullptr, DEL::ValType::REAL,    $1); }
    | HEX_LITERAL                { $$ = new DEL::AST(DEL::NodeType::VAL, nullptr, nullptr, DEL::ValType::INTEGER, std::to_string(std::strtoul($1.c_str(), 0, 16))); }
    | identifiers                { $$ = new DEL::AST(DEL::NodeType::ID,  nullptr, nullptr, DEL::ValType::STRING,  $1); }
    ;

primary_char
   : CHAR_LITERAL                { $$ = new DEL::AST(DEL::NodeType::VAL, nullptr, nullptr, DEL::ValType::CHAR,    $1); }
   | identifiers                 { $$ = new DEL::AST(DEL::NodeType::ID,  nullptr, nullptr, DEL::ValType::STRING,  $1); }
   ;

identifiers
   :  IDENTIFIER                 { $$ = $1;      } 
   |  IDENTIFIER DOT identifiers { $$ = $1 + "." + $3; }
   ;

assignment
   : INT  identifiers ASSIGN expression   SEMI { $$ = new DEL::Assignment(DEL::ValType::INTEGER, $2, $4); $$->set_line_no($5); }
   | REAL identifiers ASSIGN expression   SEMI { $$ = new DEL::Assignment(DEL::ValType::REAL,    $2, $4); $$->set_line_no($5); }
   | CHAR identifiers ASSIGN primary_char SEMI { $$ = new DEL::Assignment(DEL::ValType::CHAR,    $2, $4); $$->set_line_no($5); }
   ;

reassignment
   : identifiers ASSIGN expression   SEMI     { $$ = new DEL::Assignment(DEL::ValType::REQ_CHECK, $1, $3); $$->set_line_no($4); }
   ;

return_stmt
   : RETURN expression SEMI { $$ = new DEL::ReturnStmt($2); $$->set_line_no($3); }
   | RETURN SEMI            { $$ = new DEL::ReturnStmt();   $$->set_line_no($2); }
   ;

if_stmt
   : IF LEFT_PAREN expression RIGHT_PAREN block elif_stmt { $$ = new DEL::If(DEL::IfType::IF, $3, $5, $6, $4);      }
   | IF LEFT_PAREN expression RIGHT_PAREN block else_stmt { $$ = new DEL::If(DEL::IfType::IF, $3, $5, $6, $4);      }
   | IF LEFT_PAREN expression RIGHT_PAREN block           { $$ = new DEL::If(DEL::IfType::IF, $3, $5, nullptr, $4); }
   ;

elif_stmt
   : ELIF LEFT_PAREN expression RIGHT_PAREN block elif_stmt  { $$ = new DEL::If(DEL::IfType::ELIF, $3, $5, $6, $4);      }
   | ELIF LEFT_PAREN expression RIGHT_PAREN block else_stmt  { $$ = new DEL::If(DEL::IfType::ELIF, $3, $5, $6, $4);      }
   | ELIF LEFT_PAREN expression RIGHT_PAREN block            { $$ = new DEL::If(DEL::IfType::ELIF, $3, $5, nullptr, $4); }
   ;

else_stmt
   : ELSE block   { $$ = new DEL::If(DEL::IfType::ELSE, 
                                     new DEL::AST(DEL::NodeType::VAL, nullptr, nullptr, DEL::ValType::INTEGER, "1"), 
                                     $2, 
                                     nullptr, 
                                     $1); 
                     // We create an "always true" statement so we can leverage elseif code, while still
                     // treating this as an "else"
                  }
   ;

stmt
   : assignment    { $$ = $1; }
   | reassignment  { $$ = $1; }
   | return_stmt   { $$ = $1; }
   | direct_function_call { $$ = $1;}
   | if_stmt { $$ = $1; }
   ;

multiple_statements
   : stmt                     { $$ = std::vector<DEL::Element*>(); $$.push_back($1); }
   | multiple_statements stmt { $1.push_back($2); $$ = $1; }
   ;

block 
   : LEFT_BRACKET multiple_statements RIGHT_BRACKET { $$ = $2; }
   | LEFT_BRACKET RIGHT_BRACKET                     { $$ = std::vector<DEL::Element*>(); }
   ;

recv_params
   : value_types IDENTIFIER   { r_params.clear(); r_params.push_back({static_cast<DEL::ValType>($1), $2}); }
   | recv_params COMMA value_types IDENTIFIER {r_params.push_back({static_cast<DEL::ValType>($3), $4});}
   ;

call_item
   : IDENTIFIER   { $$ = new DEL::FunctionParam(DEL::ValType::REQ_CHECK, $1);  }
   | INT_LITERAL  { $$ = new DEL::FunctionParam(DEL::ValType::INTEGER,   $1);  }
   | REAL_LITERAL { $$ = new DEL::FunctionParam(DEL::ValType::REAL,      $1);  }
   | CHAR_LITERAL { $$ = new DEL::FunctionParam(DEL::ValType::CHAR,      $1);  }
   ;

call_params
   : call_item { c_params.clear(); c_params.push_back(*($1)); delete $1; }
   | call_params COMMA call_item   { c_params.push_back(*($3)); delete $3; }
   ;

value_types
   : INT    { $$ = static_cast<int>(DEL::ValType::INTEGER); }
   | REAL   { $$ = static_cast<int>(DEL::ValType::REAL   ); }
   | CHAR   { $$ = static_cast<int>(DEL::ValType::CHAR   ); }
   | NIL    { $$ = static_cast<int>(DEL::ValType::NONE   ); }
   ;

function_stmt
   : DEF identifiers LEFT_PAREN RIGHT_PAREN ARROW value_types block             { $$ = new DEL::Function($2, r_params, static_cast<DEL::ValType>($6), $7, $1); }
   | DEF identifiers LEFT_PAREN recv_params RIGHT_PAREN ARROW value_types block { $$ = new DEL::Function($2, r_params, static_cast<DEL::ValType>($7), $8, $1); r_params.clear(); }
   ;

direct_function_call
   : identifiers LEFT_PAREN RIGHT_PAREN SEMI             { $$ = new DEL::Call($1, c_params); $$->set_line_no($4); }
   | identifiers LEFT_PAREN call_params RIGHT_PAREN SEMI { $$ = new DEL::Call($1, c_params); $$->set_line_no($5); c_params.clear(); }
   ;



%%

void DEL::DEL_Parser::error( const location_type &l, const std::string &err_message )
{
   DEL::Errors       & error_man = driver.get_error_man_ref();
   DEL::Preprocessor & preproc   = driver.get_preproc_ref();

   // Report the error
   error_man.report_syntax_error(
         l.begin.line,                     // Line where issue appeared
         l.begin.column,                   // Column where issue appeared
         err_message,                      // Bison error information
         preproc.fetch_line(l.begin.line)  // The user line where issue appeared
   );
}