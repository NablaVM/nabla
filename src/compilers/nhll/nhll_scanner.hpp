#ifndef __NHLLSCANNER_HPP__
#define __NHLLSCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "nhll_parser.tab.hh"
#include "location.hh"

namespace NHLL
{
   //! \brief Fwd for driver
   class NHLL_Driver;

   //! \brief The NHLL Scanner object that will act as the lexer
   class NHLL_Scanner : public yyFlexLexer
   {
   public:
      
      //! \brief Create the scanner
      //! \param in The stream (in) to begin parsing 
      //! \param driver The driver object that will manage items found by the scanner 
      NHLL_Scanner(std::istream *in, NHLL_Driver& driver) : yyFlexLexer(in), driver(driver){};

      //! \brief Deconstruct
      virtual ~NHLL_Scanner() {};

      //! \brief Get rid of override virtual function warning
      using FlexLexer::yylex;

      //! \brief From yyFlexLexer
      //! \note Method body created by flex in nhll_lexer.yy.cc
      virtual int yylex( NHLL::NHLL_Parser::semantic_type * const lval,  NHLL::NHLL_Parser::location_type *location );

   private:
      NHLL::NHLL_Parser::semantic_type *yylval = nullptr;
      NHLL_Driver &driver;
   };

}

#endif
