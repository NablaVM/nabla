#ifndef __DELSCANNER_HPP__
#define __DELSCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "del_parser.tab.hh"
#include "location.hh"

namespace DEL
{
   //! \brief Fwd for driver
   class DEL_Driver;

   //! \brief The DEL Scanner object that will act as the lexer
   class DEL_Scanner : public yyFlexLexer
   {
   public:
      
      //! \brief Create the scanner
      //! \param in The stream (in) to begin parsing 
      //! \param driver The driver object that will manage items found by the scanner 
      DEL_Scanner(std::istream *in, DEL_Driver& driver) : yyFlexLexer(in), driver(driver){};

      //! \brief Deconstruct
      virtual ~DEL_Scanner() {};

      //! \brief Get rid of override virtual function warning
      using FlexLexer::yylex;

      //! \brief From yyFlexLexer
      //! \note Method body created by flex in del_lexer.yy.cc
      virtual int yylex( DEL::DEL_Parser::semantic_type * const lval,  DEL::DEL_Parser::location_type *location );

   private:
      DEL::DEL_Parser::semantic_type *yylval = nullptr;
      DEL_Driver &driver;
   };

}

#endif
