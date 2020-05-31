%skeleton "lalr1.cc"
%require  "3.0"
%debug 
%defines 
%define api.namespace {NHLL}

%define api.parser.class {NHLL_Parser}

%code requires{
   
   namespace NHLL 
   {
      class NHLL_Driver;
      class NHLL_Scanner;
      class NhllFunction;
      class NhllElement;
      class ConditionalExpression;
      enum class Conditionals;
   }

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}

%parse-param { NHLL_Scanner  &scanner  }
%parse-param { NHLL_Driver  &driver  }

%code{
   #include <iostream>
   #include <cstdlib>
   #include <fstream>
   #include <stdint.h>
   #include <vector>
   
   #include "nhll.hpp"
   #include "nhll_driver.hpp"
   #include "nhll_postfix.hpp"

   // Elements in a 'check' statement
   std::vector< NHLL::NhllElement* > check_list;

   // Parameters on the receiving end of a call
   std::vector< NHLL::FunctionParam > r_paramaters;

   // Paramaters on the caller end of a call
   std::vector< std::string > s_paramaters;

#undef yylex
#define yylex scanner.yylex
}

%define api.value.type variant
%define parse.assert

%type<std::string> expression;
%type<std::string> term;
%type<std::string> factor;
%type<std::string> primary;
%type<std::string> passable;
%type<std::string> identifiers;
%type<int> data_prim;

%type<NHLL::NhllElement*> stmt;
%type<NHLL::NhllElement*> asm_stmt;
%type<NHLL::NhllElement*> let_stmt;
%type<NHLL::NhllElement*> check_stmt;
%type<NHLL::NhllElement*> reassign_stmt;
%type<NHLL::NhllElement*> global_stmt;
%type<NHLL::NhllElement*> call_stmt;
%type<NHLL::NhllElement*> while_stmt;
%type<NHLL::NhllElement*> loop_stmt;
%type<NHLL::NhllElement*> break_stmt;
%type<NHLL::NhllElement*> function_stmt;
%type<NHLL::NhllElement*> yield_stmt;
%type<NHLL::NhllElement*> exit_stmt;
%type<NHLL::NhllElement*> return_stmt;
%type<NHLL::NhllElement*> check_cond;

%type<std::vector<NhllElement*>> multiple_statements;
%type<std::vector<NhllElement*>> block;

%token FUNC_DECL LET WHILE LOOP INT NIL REAL STR RET_ARROW BREAK GLOBAL
%token RETURN YIELD EXIT CHECK
%token LTE GTE LT GT EQ NE COR CAND
%token LEFT_SH RIGHT_SH OR XOR AND NOT
%token <std::string> ASM
%token <std::string> STRING_LITERAL
%token <std::string> INTEGER_LITERAL
%token <std::string> REAL_LITERAL
%token <std::string> IDENTIFIER
%token               END    0     "end of file"
%locations
%start start

%%

start 
   : END 
   | input END
   ; 

input
   : function_stmt       { driver.build_element($1); }
   | input function_stmt { driver.build_element($2); }
   | global_stmt         { driver.build_element($1); }
   | input global_stmt   { driver.build_element($2); }
   ;

multiple_statements 
   : stmt                        { $$ = std::vector<NhllElement*>(); $$.push_back($1); }     // Create the list to return, and add the statement
   | multiple_statements stmt    { $1.push_back($2); $$ = $1; }                              // Add the statement to the list 
   ;

identifiers
   :  IDENTIFIER                 { $$ = $1;      } 
   |  IDENTIFIER '.' identifiers { $$ = $1 + "." + $3; }
   ;

expression
   : term                        { $$ = $$ + $1; }
   | expression '+' term         { $$ = $$ + $1 + (char)Postfix::POP::ADD + $3; }
   | expression '-' term         { $$ = $$ + $1 + (char)Postfix::POP::SUB + $3; }
   | expression LTE term         { $$ = $$ + $1 + (char)Postfix::POP::LTE + $3; }
   | expression GTE term         { $$ = $$ + $1 + (char)Postfix::POP::GTE + $3; }
   | expression GT  term         { $$ = $$ + $1 + (char)Postfix::POP::GT  + $3; }
   | expression LT  term         { $$ = $$ + $1 + (char)Postfix::POP::LT  + $3; }
   | expression EQ  term         { $$ = $$ + $1 + (char)Postfix::POP::EQ  + $3; }
   | expression NE  term         { $$ = $$ + $1 + (char)Postfix::POP::NE  + $3; }
   ;

term
   : factor
   | term '*' factor             { $$ = $$ + $1 + (char)Postfix::POP::MUL + $3; } // Postfix class uses special symbols for some things
   | term '/' factor             { $$ = $$ + $1 + (char)Postfix::POP::DIV + $3; } // so we just add them by name here
   | term '^' factor             { $$ = $$ + $1 + (char)Postfix::POP::POW + $3; } // For instance, this usually doesn't mean power in languages
   | term '%' factor             { $$ = $$ + $1 + (char)Postfix::POP::MOD + $3; } // I think thats dumb, so I made ^ mean power
   | term LEFT_SH  factor        { $$ = $$ + $1 + (char)Postfix::POP::LSH + $3; }
   | term RIGHT_SH factor        { $$ = $$ + $1 + (char)Postfix::POP::RSH + $3; }
   | term XOR factor             { $$ = $$ + $1 + (char)Postfix::POP::XOR + $3; }
   | term OR factor              { $$ = $$ + $1 + (char)Postfix::POP::OR  + $3; }
   | term AND factor             { $$ = $$ + $1 + (char)Postfix::POP::AND + $3; }
   | term COR  factor            { $$ = $$ + $1 + (char)Postfix::POP::COR  + $3;}
   | term CAND factor            { $$ = $$ + $1 + (char)Postfix::POP::CAND + $3;}
   ;

factor
   : primary                     { $$ = $$ + $1; }
   | '(' expression ')'          { $$ = $$ + "(" + $2 + ")"; }
   | NOT factor                  { $$ = $$ + (char)Postfix::POP::NOT + $2; }
   ;

data_prim
   :  INT  { $$ = static_cast<int>(NHLL::DataPrims::INT ); }
   |  REAL { $$ = static_cast<int>(NHLL::DataPrims::REAL); }
   |  STR  { $$ = static_cast<int>(NHLL::DataPrims::STR ); }
   |  NIL  { $$ = static_cast<int>(NHLL::DataPrims::NIL ); }
   ;

recv_paramaters 
   : data_prim IDENTIFIER                       { r_paramaters.push_back(FunctionParam { static_cast<DataPrims>($1), $2 }); }
   | recv_paramaters ',' data_prim IDENTIFIER   { r_paramaters.push_back(FunctionParam { static_cast<DataPrims>($3), $4 }); }
   ;

send_paramaters 
   : passable                                   { s_paramaters.push_back($1); }
   | send_paramaters ',' passable               { s_paramaters.push_back($3); }
   ;

primary
    : INTEGER_LITERAL                     { $$ = $1;       }
    | '-' INTEGER_LITERAL                 { $$ = "-" + $2; }   // because -?[0-9]+ Would only match negatives ? 
    | REAL_LITERAL                        { $$ = $1;       }
    | identifiers                         { $$ = $1;       }
    ;

passable
   : identifiers     { $$ = $1; }
   | INTEGER_LITERAL { $$ = $1; }
   | REAL_LITERAL    { $$ = $1; }
   | STRING_LITERAL  { $$ = $1; }
   ;

stmt
   : let_stmt      { $$ = $1; }
   | reassign_stmt { $$ = $1; }
   | global_stmt   { $$ = $1; }
   | call_stmt     { $$ = $1; }
   | while_stmt    { $$ = $1; }
   | loop_stmt     { $$ = $1; }
   | break_stmt    { $$ = $1; }
   | asm_stmt      { $$ = $1; }
   | yield_stmt    { $$ = $1; }
   | return_stmt   { $$ = $1; }
   | exit_stmt     { $$ = $1; }
   | check_stmt    { $$ = $1; }
   ;

global_stmt
   : GLOBAL identifiers '=' expression     { $$ = driver.create_global_statement($2, $4); }
   | GLOBAL identifiers '=' STRING_LITERAL { $$ = driver.create_global_statement($2, $4, false); }
   ;

block 
   : '{' multiple_statements '}' { $$ = $2; }   // Return the statement list
   | '{' '}'                     { $$ = std::vector<NhllElement*>(); }
   ;

asm_stmt
   : ASM    { $$ = driver.create_asm_statement($1); }
   ;

let_stmt
   : LET identifiers '=' expression       { $$ = driver.create_let_statement($2, $4);        }
   | LET identifiers '=' STRING_LITERAL   { $$ = driver.create_let_statement($2, $4, false); }
   ;

reassign_stmt
   : identifiers '=' expression           { $$ = driver.create_reassign_statement($1, $3);        }
   | identifiers '=' STRING_LITERAL       { $$ = driver.create_reassign_statement($1, $3, false); }
   ;

yield_stmt
   : YIELD expression      { $$ = driver.create_leave_statement($2, false, true);  }
   | YIELD STRING_LITERAL  { $$ = driver.create_leave_statement($2, false, false); }
   ;

return_stmt
   : RETURN expression     { $$ = driver.create_leave_statement($2, true,  true);  }
   | RETURN STRING_LITERAL { $$ = driver.create_leave_statement($2, true,  false); }
   ;

exit_stmt
   :  EXIT                  { $$ = driver.create_exit_statement();                  }
   ;

call_stmt
   : identifiers '(' ')'                  { $$ = driver.create_call_statement($1, std::vector<std::string>()); }
   | identifiers '(' send_paramaters ')'  { $$ = driver.create_call_statement($1, s_paramaters); s_paramaters.clear(); }
   ;

while_stmt
   :  WHILE '(' expression ')' block  { $$ = driver.create_while_statement($3, $5); }
   ;

loop_stmt
   : LOOP '.' IDENTIFIER block            { $$ = driver.create_loop_statement($3, $4); }
   ;

break_stmt
   : BREAK IDENTIFIER                     { $$ = driver.create_break_statement($2); }
   ;

function_stmt
   : FUNC_DECL identifiers '(' ')' RET_ARROW data_prim block                 { $$ = driver.create_function_statement($2, std::vector<FunctionParam>(), static_cast<DataPrims>($6), $7); }
   | FUNC_DECL identifiers '(' recv_paramaters ')' RET_ARROW data_prim block { $$ = driver.create_function_statement($2, r_paramaters, static_cast<DataPrims>($7), $8); r_paramaters.clear(); }
   ;
   
check_cond
   :  '[' expression ']' block  { $$ = driver.create_check_condition($2, $4); }
   ;

checks
   : check_cond            { check_list.push_back($1); }
   | checks check_cond     { check_list.push_back($2); }
   ;

check_final
   : '[' ']' block  { 
                              // The final 'else' in the check statement we build a ' 1==1' condition that should be true unless
                              // the universe has bigger issues.
                              std::string condition = "1" + std::string( 1, (char)Postfix::POP::EQ ) + "1";
                              check_list.push_back(
                                 driver.create_check_condition(condition ,  $3)
                              );
                           }
   ;

check_stmt
   :  CHECK '{' checks check_final '}' { $$ = driver.create_check_statement(check_list); check_list.clear(); }
   ;

%%

void NHLL::NHLL_Parser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
