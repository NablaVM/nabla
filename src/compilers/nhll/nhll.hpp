/*
    I REALLY don't want to go through all of these objects and write documentation so I'm pushing it off for later. 
    In a brief description, all of these objects are representations of the things that are found in the NHLL. While loops,
    loops, check blocks, variable assignments, etc.
*/

#ifndef NHLL_LANGUAGE_HPP
#define NHLL_LANGUAGE_HPP

#include <string>
#include <vector>
#include <memory>

namespace NHLL
{
    class NhllVisitor;

    class NhllElement
    {
    public:
        virtual ~NhllElement() = default;
        virtual void visit(NhllVisitor &visitor) = 0;
    };

    typedef std::vector<NHLL::NhllElement*> ElementList;


    enum class Conditionals
    {
        LT, GT, LTE, GTE, NE, EQ, AND, OR, NONE
    };

    enum class ConditialExpressionType
    {
        ID, INT, REAL, EXPR
    };

    enum class DataPrims
    {
        INT, REAL, STR, NIL
    };

    static std::string DataPrims_to_string(DataPrims dp)
    {
         switch(dp)
         {
            case DataPrims::INT:  return "int"; 
            case DataPrims::REAL: return "real";
            case DataPrims::STR:  return "str"; 
            case DataPrims::NIL:  return "nil"; 
            default: break;
         }
        return "Invalid data prim given for to_string";
    }

    //
    //
    //
    class UseStmt : public NhllElement
    {
    public:
        UseStmt() {}

        UseStmt(std::string module, std::string as_name) 
                            : module(module),
                              as_name(as_name){}

        UseStmt(UseStmt *o) : module(o->module),
                              as_name(o->as_name){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string module;
        std::string as_name;
    };

    //
    //
    //
    class LetStmt : public NhllElement
    {
    public:
        LetStmt() {}

        LetStmt(std::string lhs, std::string rhs, bool is_expr) 
                            : identifier(lhs),
                              set_to(rhs),
                              is_expr(is_expr){}

        LetStmt(LetStmt *o) : identifier(o->identifier),
                              set_to(o->set_to),
                              is_expr(o->is_expr){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string identifier;
        std::string set_to;
        bool is_expr;
    };

    //
    //
    //
    class GlobalStmt : public NhllElement
    {
    public:
        GlobalStmt() {}

        GlobalStmt(std::string lhs, std::string rhs, bool is_expr) 
                            : identifier(lhs),
                              set_to(rhs),
                              is_expr(is_expr){}

        GlobalStmt(GlobalStmt *o) : identifier(o->identifier),
                              set_to(o->set_to),
                              is_expr(o->is_expr){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string identifier;
        std::string set_to;
        bool is_expr;
    };

    //
    //
    //
    class CallStmt : public NhllElement
    {
    public:
        CallStmt() {}

        CallStmt(std::string function, 
                 std::vector< std::string> params) 
                            : function(function),
                              params(params){}

        CallStmt(CallStmt *o) : function(o->function),
                                params(o->params){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string function;
        std::vector< std::string> params;
    };

    //
    //
    //
    class ReAssignStmt : public NhllElement
    {
    public:
        ReAssignStmt() {}

        ReAssignStmt(std::string lhs, std::string rhs, bool is_expr) 
                            : identifier(lhs),
                              set_to(rhs),
                              is_expr(is_expr){}

        ReAssignStmt(ReAssignStmt *o) : identifier(o->identifier),
                              set_to(o->set_to),
                              is_expr(o->is_expr){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string identifier;
        std::string set_to;
        bool is_expr;
    };

    //
    //
    //
    class LoopStmt : public NhllElement
    {
    public:
        LoopStmt() {}
        LoopStmt(std::string id, ElementList el) : id(id), elements(el){}
        LoopStmt(LoopStmt * o) : id(o->id), elements(o->elements) {}

        virtual void visit(NhllVisitor &visitor) override;

        std::string id;
        ElementList elements;
    };

    //
    //
    //
    class BreakStmt : public NhllElement
    {
    public:
        BreakStmt() {}
        BreakStmt(std::string id) : id(id) {}
        BreakStmt(BreakStmt *  o) : id(o->id){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string id;
    };

    //
    //
    //
    class AsmStmt : public NhllElement
    {
    public:
        AsmStmt() {}
        AsmStmt(std::vector<std::string> asm_block) : asm_block(asm_block) {}
        AsmStmt(AsmStmt *  o) : asm_block(o->asm_block){}

        virtual void visit(NhllVisitor &visitor) override;

        std::vector<std::string> asm_block;
    };

    //
    //
    //
    class ExitStmt : public NhllElement
    {
    public:
        ExitStmt() {}
        ExitStmt(ExitStmt *  o){}

        virtual void visit(NhllVisitor &visitor) override;
    };

    //
    //
    //
    class LeaveStmt : public NhllElement
    {
    public:
        enum class Variant
        {
            YIELD,
            RETURN
        };

        LeaveStmt() {}
        LeaveStmt(std::string value, Variant variant, bool is_expr) : value(value), 
                                                                      variant(variant), 
                                                                      is_expr(is_expr) {}
        LeaveStmt(LeaveStmt *  o) :value(o->value), 
                                   variant(o->variant), 
                                   is_expr(o->is_expr) {}


        virtual void visit(NhllVisitor &visitor) override;


        std::string value;
        Variant variant;
        bool is_expr;
    };

    //
    //
    //
    class WhileStmt : public NhllElement
    {
    public:
        WhileStmt() {}

        WhileStmt(std::string conditional_expression, ElementList el)
        {
            condition = conditional_expression;
            elements = el;
        }

        WhileStmt(WhileStmt *o)
        {
            condition = o->condition;
            elements = o->elements;
        }

        virtual void visit(NhllVisitor &visitor) override;

        std::string condition;
        ElementList elements;
    };

    struct FunctionParam
    {
        DataPrims type;
        std::string name;
    };

    //
    //
    //
    class NhllFunction : public NhllElement
    {
    public:
        NhllFunction() {}

        NhllFunction(std::string name, 
                     std::vector<FunctionParam> params,
                     DataPrims return_type,
                     ElementList el) 
                            : name(name),
                              params(params),
                              return_type(return_type),
                              elements(el){}

        NhllFunction(NhllFunction *o) : name(o->name),
                                        params(o->params),
                                        return_type(o->return_type),
                                        elements(o->elements){}

        virtual void visit(NhllVisitor &visitor) override;

        std::string name;
        std::vector<FunctionParam> params;
        DataPrims return_type;
        ElementList elements;
    };

    //
    //
    //
    class CheckCondition : public NhllElement
    {
    public:
        CheckCondition() {}

        CheckCondition(std::string conditional_expression, ElementList el)
        {
            condition = conditional_expression;
            elements = el;
        }

        CheckCondition(CheckCondition *o)
        {
            condition = o->condition;
            elements = o->elements;
        }

        virtual void visit(NhllVisitor &visitor) override;

        std::string condition;
        ElementList elements;
    };

    //
    //
    //
    class CheckStmt : public NhllElement
    {
    public:
        CheckStmt() {}

        CheckStmt(ElementList el)
        {
            elements = el;
        }

        CheckStmt(CheckStmt *o)
        {
            elements = o->elements;
        }

        virtual void visit(NhllVisitor &visitor) override;

        ElementList elements;
    };
    /*
            Visitor is last so we don't  have to fwd decl all the things
    */
    class NhllVisitor
    {
    public:
        virtual void accept(AsmStmt &stmt) = 0;
        virtual void accept(LetStmt &stmt) = 0;
        virtual void accept(ReAssignStmt & stmt) = 0;
        virtual void accept(GlobalStmt & stmt) = 0;
        virtual void accept(WhileStmt &stmt) = 0;
        virtual void accept(LoopStmt &stmt) = 0;
        virtual void accept(BreakStmt &stmt) = 0;
        virtual void accept(CallStmt &stmt) = 0;
        virtual void accept(NhllFunction &stmt) = 0;
        virtual void accept(LeaveStmt &stmt) = 0;
        virtual void accept(ExitStmt &stmt) = 0;
        virtual void accept(CheckCondition &stmt) = 0;
        virtual void accept(CheckStmt &stmt) = 0;
    };
}

#endif