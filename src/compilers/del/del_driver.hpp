#ifndef __DELDRIVER_HPP__
#define __DELDRIVER_HPP__ 1

#include <string>
#include <cstddef>
#include <istream>
#include <vector>

#include "Ast.hpp"
#include "Analyzer.hpp"
#include "Errors.hpp"
#include "Codegen.hpp"
#include "Tracker.hpp"
#include "SymbolTable.hpp"
#include "Memory.hpp"
#include "del_scanner.hpp"
#include "del_parser.tab.hh"

namespace DEL
{
   //!\brief The DEL Driver object
   class DEL_Driver
   {
   public:

      const std::string DEFAULT_ASM_OUT = "del.asm";
      const std::string DEFAULT_BIN_OUT = "del.out";

      DEL_Driver();

      //! \brief Deconstruct
      virtual ~DEL_Driver();
      
      //! \brief Parse from a file
      void parse( const char * const filename );

      //! \brief Parse from an istream
      void parse( std::istream &iss );

      //! \brief Inc line count
      void inc_line();

      //! \brief Pass through to analyzer to check code before calling codegen
      //! \param function The function to build
      void build_function(Function *function);

      //! \brief Indicate that parsing is completed
      void indicate_complete();

   private:

      DEL::Tracker tracker;            // Line / col tracker
      DEL::Memory  memory_man;         // Memory manager
      DEL::Errors error_man;           // Error manager
      DEL::SymbolTable symbol_table;   // Symbols found in the language
      DEL::Codegen code_gen;           // Code generator
      DEL::Analyzer analyzer;          // Code analyzer

      std::ostream& print(std::ostream &stream);
      
      void parse_helper( std::istream &stream );

      DEL::DEL_Parser  *parser  = nullptr;
      DEL::DEL_Scanner *scanner = nullptr;


      std::string evaluate(AST * ast);

   };

}
#endif
