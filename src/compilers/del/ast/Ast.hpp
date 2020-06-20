#ifndef DEL_AST_HPP
#define DEL_AST_HPP

#include "Types.hpp"
#include <vector>

namespace DEL
{
    //
    //  A node
    //
    class Node
    {
    public:
        Node(NodeType type) : node_type(type), val_type(ValType::NONE) {}

        NodeType node_type;
        ValType val_type;
        std::string value;
    };

    //
    //  A tree
    //
    class AST : public Node 
    {
    public: 
        AST() : Node(NodeType::ROOT), l(nullptr), r(nullptr) {}
        AST(NodeType type, AST*lhs, AST*rhs) : Node(type),
                                               l(lhs), 
                                               r(rhs) {}

        AST(NodeType type, AST*lhs, AST*rhs, ValType v, std::string a): Node(type),
                                               l(lhs), 
                                               r(rhs) 
        {
            this->val_type = v;
            this->value = a;
        }

        AST * l;
        AST * r;
    };

    //
    //  A range of values
    //
    class Range
    {
    public: 
        // Create a range
        Range(ValType type, std::string from, std::string to, int line) : 
            type(type), from(from), to(to), line_no(line){}

        ValType type;
        std::string from;
        std::string to;
        int line_no;
    };
    
    //
    // Fwd for a visitor
    //
    class Visitor;

    //
    // A visitable element
    //
    class Element
    {
    public:
        virtual ~Element() = default;
        virtual void visit(Visitor &visitor) = 0;

        void set_line_no(int line)
        {
            line_no = line;
        }

        int line_no;
    };

    // A list of elements
    typedef std::vector<Element*> ElementList;

    //
    //  An assignment
    //
    class Assignment : public Element
    {
    public:
        Assignment(ValType type, std::string lhs, AST * rhs) : data_type(type),
                                                                lhs(lhs),
                                                                rhs(rhs){}
        virtual void visit(Visitor &visit) override;

        ValType data_type;
        std::string lhs;
        AST * rhs;
    };

    //
    //  If statement
    //
    class If : public Element
    {
    public:
        If(IfType type, AST * expr, ElementList elements, Element * trail, int line) : type(type), expr(expr), element_list(elements), trail(trail)
        {
            line_no = line;
        }

        virtual void visit(Visitor &visit) override;

        void set_var_name(std::string var)
        {
            var_name = var;
        }

        IfType type;
        AST * expr;
        ElementList element_list;
        Element * trail;
        std::string var_name;
    };

    //
    //  A return statement
    //
    class ReturnStmt : public Element
    {
    public:
        ReturnStmt(AST * rhs) : data_type(ValType::REQ_CHECK), rhs(rhs){}
        ReturnStmt() : data_type(ValType::NONE), rhs(nullptr){}

        virtual void visit(Visitor &visit) override;

        ValType data_type;
        AST * rhs;
    };

    //
    //  A for loop
    //
    class ForLoop : public Element
    {
    public: 
        ForLoop(std::string id, Range * range, std::string step, ElementList elements) : 
            id(id), range(range), step(step), elements(elements){}

        virtual void visit(Visitor &visit) override;
        
        std::string id;
        Range * range;
        std::string step;
        ElementList elements;
    };

    //
    //  A call 
    //
    class Call : public Element, public AST
    {
    public:
        // Creation for something to use it as an element
        Call(std::string name, std::vector<FunctionParam> params) : 
            AST(NodeType::CALL, nullptr, nullptr), 
            name(name), params(params) {}

        // Creation for something to use it as part of an AST
        Call(std::string name, std::vector<FunctionParam> params, AST * lhs, AST * rhs) : 
            AST(NodeType::CALL, lhs, rhs, ValType::REQ_CHECK, name),
            name(name), params(params){}

        // A call created given a line number
        Call(std::string name, std::vector<FunctionParam> params, AST * lhs, AST * rhs, int line_no) : 
            AST(NodeType::CALL, lhs, rhs, ValType::REQ_CHECK, name),
            name(name), params(params)
            {
                this->line_no = line_no;
            }

        // Let the visitor visit us
        virtual void visit(Visitor &visit) override;

        std::string name;
        std::vector<FunctionParam> params;
    };


    //
    //  A function
    //
    class Function
    {
    public:
        Function(std::string name, std::vector<FunctionParam> params, ValType return_type, ElementList elements, int line) :
            name(name), params(params), return_type(return_type), elements(elements), line_no(line){}

        std::string name;
        std::vector<FunctionParam> params;
        ValType return_type;
        ElementList elements;
        int line_no;
    };

    //
    //  Visitor 
    //
    class Visitor
    {
    public:
        virtual void accept(Assignment &stmt) = 0;
        virtual void accept(ReturnStmt &stmt) = 0;
        virtual void accept(Call       &stmt) = 0;
        virtual void accept(If         &stmt) = 0;
        virtual void accept(ForLoop    &stmt) = 0;
    };
}

#endif