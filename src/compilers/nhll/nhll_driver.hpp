#ifndef __NHLLDRIVER_HPP__
#define __NHLLDRIVER_HPP__ 1

#include <string>
#include <cstddef>
#include <istream>
#include <vector>
#include <memory>
#include <stack>

#include "nhll.hpp"
#include "nhll_scanner.hpp"
#include "nhll_parser.tab.hh"
#include "nhll_postfix.hpp"

namespace NHLL
{
   //! \brief CodeGen Fwd
   class CodeGen;
   
   //!\brief The NHLL Driver object, used to construct elements found by the lexer / parser
   class NHLL_Driver : public NhllVisitor
   {
   public:

      //! \brief Construct the driver 
      //! \param code_generator The code generator object that the driver will call into
      //!        To have the bytecode generated based on the elements within the driver
      NHLL_Driver(NHLL::CodeGen &code_generator);

      //! \brief Deconstruct
      virtual ~NHLL_Driver();
      
      //! \brief Parse from a file
      void parse( const char * const filename );

      //! \brief Parse from an istream
      void parse( std::istream &iss );

      //! \brief Indicate to the driver that everything is completed
      //! \retval True completion execution was able to be run, False otherwise
      bool indicate_complete();

      //! \brief Trigger the building of a given element
      //! \param el The element to build 
      //! \note This triggers visits contained by the element and each accept method 
      //!       calls into the codegen to mark the construction of individual parts
      void build_element(NHLL::NhllElement* el);

      //! \brief Create the particular element
      NHLL::NhllElement* create_asm_statement(std::string asm_block);

      //! \brief Create the particular element
      NHLL::NhllElement* create_function_statement(std::string name, std::vector<FunctionParam> params, DataPrims ret, ElementList elements);

      //! \brief Create the particular element
      NHLL::NhllElement* create_decl_integer(std::string lhs, std::string rhs, bool is_expression=true);

      //! \brief Create the particular element
      NHLL::NhllElement* create_decl_real(std::string lhs, std::string rhs, bool is_expression=true);

      //! \brief Create the particular element
      NHLL::NhllElement* create_decl_string(std::string lhs, std::string rhs, uint64_t max_size);

      //! \brief Create the particular element
      NHLL::NhllElement* create_reassign_statement(std::string lhs, std::string rhs, bool is_expression=true);

      //! \brief Create the particular element
      NHLL::NhllElement* create_global_statement(std::string lhs, std::string rhs, bool is_expression=true);

      //! \brief Create the particular element
      NHLL::NhllElement* create_while_statement(std::string cond_expr, ElementList elements);

      //! \brief Create the particular element
      NHLL::NhllElement* create_loop_statement(std::string id, ElementList elements);

      //! \brief Create the particular element
      NHLL::NhllElement* create_break_statement(std::string id);

      //! \brief Create the particular element
      NHLL::NhllElement* create_call_statement(std::string function, std::vector<std::string> params);

      //! \brief Create the particular element
      NHLL::NhllElement* create_leave_statement(std::string value, bool is_return, bool is_expression=true);

      //! \brief Create the particular element
      NHLL::NhllElement* create_exit_statement();

      //! \brief Create the particular element
      NHLL::NhllElement* create_check_condition(std::string cond_expr, ElementList elements);

      //! \brief Create the particular element
      NHLL::NhllElement* create_check_statement(ElementList elements);

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(AsmStmt &stmt) override;

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(DeclInteger &stmt) override;

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(DeclReal &stmt) override;

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(DeclString &stmt) override;

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(ReAssignStmt &stmt) override;

      //! \brief Visit a set statement, triggers code generation
      virtual void accept(GlobalStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(WhileStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(LoopStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(BreakStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(CallStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(NhllFunction &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(LeaveStmt &stmt) override;

      //! \brief Visit a while statement, triggers code generation
      virtual void accept(ExitStmt &stmt) override;
      
      //! \brief Visit a while statement, triggers code generation
      virtual void accept(CheckCondition &stmt) override;
      
      //! \brief Visit a while statement, triggers code generation
      virtual void accept(CheckStmt &stmt) override;

      //! \brief Print driver - This will be updated later once we have some information regarding 
      //!        The actual build process so the user can use -v or something and get the particulars of the
      //!        build. Right now it doesn't do much as the information intended to presented hasn't been
      //!        accumulated. 
      std::ostream& print(std::ostream &stream);
      
   private:

      void parse_helper( std::istream &stream );

      NHLL::NHLL_Parser  *parser  = nullptr;
      NHLL::NHLL_Scanner *scanner = nullptr;

      NHLL::CodeGen & code_generator;
   };

}
#endif
