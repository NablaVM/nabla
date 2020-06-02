#ifndef NABLA_CODE_GEN_HELPERS_HPP
#define NABLA_CODE_GEN_HELPERS_HPP

#include <string>
#include <sstream>
#include <stack>
#include <iostream>
#include <cmath> 
#include "nhll_postfix.hpp"

namespace NHLL
{
namespace HELPERS 
{
    inline static bool doubles_equal(double lhs, double rhs)
    {
        double precision = 0.00001;
        if (((lhs - precision) < rhs) && 
            ((lhs + precision) > rhs))
        {
            return true;
        }
        return false;
    }

    template<typename Numeric>
    inline static bool is_number(const std::string& s)
    {
        Numeric n;
        return((std::istringstream(s) >> n >> std::ws).eof());
    }

    /*
        Precompute is for calculating global static variables at compile-time
        Global expressions can be ints or doubles, and can have all of the postfix class's 
        operations done to them. 

        This is basically a big post-fix calculator
    */
    template<typename VType>
    inline static std::string pre_compute(std::vector<Postfix::Element> elements)
    {
        std::string final_result;
        std::stack<VType> compute_stack;

        for(auto & el : elements)
        {
            switch(el.type)
            {
            case Postfix::Type::VALUE:
            {
                VType val;
                std::istringstream(el.value) >> val;
                compute_stack.push(val);
                break;
            }
            case Postfix::Type::OP:
            {
                switch(el.operation)
                {
                    case Postfix::POP::ADD :
                    {
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = lhs + rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::MUL :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = lhs * rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::SUB :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = lhs - rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::DIV :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = lhs / rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::POW :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = std::pow(lhs, rhs);
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::MOD :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = (int64_t)lhs %  (int64_t)rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::NOT :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType result = ~ (int64_t)rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::LSH :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = (int64_t)lhs << (int64_t)rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::RSH :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = (int64_t)lhs >> (int64_t)rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::OR  :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = (int64_t)lhs | (int64_t)rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::XOR :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = (int64_t)lhs ^ (int64_t)rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::AND :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = (int64_t)lhs & (int64_t)rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::LTE :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = std::islessequal(lhs, rhs);
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::GTE :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = std::isgreaterequal(lhs, rhs);
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::GT  :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = std::isgreater(lhs, rhs);
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::LT  :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = std::isless(lhs, rhs);
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::EQ  :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = doubles_equal((double)lhs, (double)rhs);
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::NE  :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = !doubles_equal((double)lhs, (double)rhs);
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::COR :
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = lhs || rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::CAND:
                    { 
                        VType rhs = compute_stack.top(); compute_stack.pop();
                        VType lhs = compute_stack.top(); compute_stack.pop();
                        VType result = lhs && rhs;
                        compute_stack.push(result);
                        break;
                    }
                    case Postfix::POP::NONE:
                    default:
                        // Really just for the compiler, but who knows, maybe it'll show up some day
                        std::cerr << "CodeGen::HELPERS::Error: Somehow an element operation wasn't matched" << std::endl;
                        exit(EXIT_FAILURE);
                        break;
                }
                break; // Break Postfix::Type::OP:
            }
            default:
                // Really just for the compiler, but who knows, maybe it'll show up some day
                std::cerr << "CodeGen::HELPERS::Error: Somehow an element type wasn't an OP or a VALUE" << std::endl;
                exit(EXIT_FAILURE);
                break;
            }
        }
        final_result = std::to_string(compute_stack.top());
        return final_result;
    }
}
}


#endif
