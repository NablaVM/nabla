/*
    This postfix conversion method was adapted from one I wrote a long time ago. I'm not proud of it. It works well, 
    but the implementation is aweful. Low hanging fruit for anyone who wants to make this better. 
*/

#include "nhll_postfix.hpp"

#include <map>
#include <stack>
#include <sstream>

namespace NHLL
{
    std::vector<Postfix::Element> Postfix::convert(std::string exp)
    {
        // https://en.wikipedia.org/wiki/Order_of_operations

        std::stack<std::string> op;
        std::string res = "";
        std::map<std::string,std::pair<int,int> > ops;
        ops["^"]  = std::make_pair(16, 1); // E
        ops["~"]  = std::make_pair(15, 0); // S
        ops["*"]  = std::make_pair(14, 0); // M
        ops["/"]  = std::make_pair(14, 0); // D
        ops["%"]  = std::make_pair(14, 0); // MOD
        ops["+"]  = std::make_pair(13, 0); // A
        ops["-"]  = std::make_pair(13, 0); // S
        ops["<"]  = std::make_pair(12, 0); // left shift
        ops[">"]  = std::make_pair(12, 0); // right shift
        ops["`"]  = std::make_pair(11, 0); // LTE
        ops[";"]  = std::make_pair(11, 0); // GTE
        ops[","]  = std::make_pair(11, 0); // GT 
        ops["#"]  = std::make_pair(11, 0); // LT 
        ops["\""] = std::make_pair(10, 0); // EQ
        ops["$"]  = std::make_pair(10, 0); // NE
        ops["&"]  = std::make_pair(9,  0); // AND - Bitwise
        ops["@"]  = std::make_pair(8,  0); // XOR
        ops["|"]  = std::make_pair(7,  0); // OR  - Bitwise
        ops[":"]  = std::make_pair(6,  0); // AND
        ops["_"]  = std::make_pair(5,  0); // OR

        std::vector<Element> element_vector;

        for(long unsigned int i = 0; i < exp.size(); i++)
        {
            std::string c_str = std::string(1, exp[i]);

            if(exp[i] == '(')
            {
                op.push(c_str);
            }
            else if(exp[i] == ')')
            {
                while(op.top() != "(")
                {
                        res += " " + op.top() + " ";
                        op.pop();
                }
                op.pop();
            }
            else if(ops.find(c_str) != ops.end())
            {
                while(!op.empty() &&
                        ((ops[c_str].second == 0 &&
                            ops[op.top()].first == ops[c_str].first) ||
                            ops[c_str].first < ops[op.top()].first))
                {
                        res += " " + op.top() + " ";
                        op.pop();
                }
                res += " ";
                op.push(c_str);
            }
            else
            {
                res += exp[i];
            }
        }
        while(!op.empty())
        {
                res += " " + op.top() + " ";
                op.pop();
        }

        // Build the element vector
        std::string tmp;
        std::stringstream ss(res);
        while (ss >> tmp)
        {
            // Sanity check
            if(tmp.size() == 0)
            {
                continue;
            }

            switch(static_cast<POP>(tmp[0]))
            {
                case POP::ADD: element_vector.push_back(Element{ tmp, Type::OP, POP::ADD }); break;
                case POP::MUL :element_vector.push_back(Element{ tmp, Type::OP, POP::MUL }); break;
                case POP::SUB :element_vector.push_back(Element{ tmp, Type::OP, POP::SUB }); break;
                case POP::DIV :element_vector.push_back(Element{ tmp, Type::OP, POP::DIV }); break;
                case POP::POW :element_vector.push_back(Element{ tmp, Type::OP, POP::POW }); break;
                case POP::MOD :element_vector.push_back(Element{ tmp, Type::OP, POP::MOD }); break;
                case POP::NOT :element_vector.push_back(Element{ tmp, Type::OP, POP::NOT }); break;
                case POP::LSH :element_vector.push_back(Element{ tmp, Type::OP, POP::LSH }); break;
                case POP::RSH :element_vector.push_back(Element{ tmp, Type::OP, POP::RSH }); break;
                case POP::OR  :element_vector.push_back(Element{ tmp, Type::OP, POP::OR  }); break;
                case POP::XOR :element_vector.push_back(Element{ tmp, Type::OP, POP::XOR }); break;
                case POP::AND :element_vector.push_back(Element{ tmp, Type::OP, POP::AND }); break;
                case POP::LTE :element_vector.push_back(Element{ tmp, Type::OP, POP::LTE }); break;
                case POP::GTE :element_vector.push_back(Element{ tmp, Type::OP, POP::GTE }); break;
                case POP::GT  :element_vector.push_back(Element{ tmp, Type::OP, POP::GT  }); break;
                case POP::LT  :element_vector.push_back(Element{ tmp, Type::OP, POP::LT  }); break;
                case POP::EQ  :element_vector.push_back(Element{ tmp, Type::OP, POP::EQ  }); break;
                case POP::NE  :element_vector.push_back(Element{ tmp, Type::OP, POP::NE  }); break;
                case POP::COR :element_vector.push_back(Element{ tmp, Type::OP, POP::COR }); break;
                case POP::CAND:element_vector.push_back(Element{ tmp, Type::OP, POP::CAND}); break;
                case POP::NONE: /* The thing wont actually come out as None, thats an us thing */ break;
                default:       element_vector.push_back(Element{ tmp, Type::VALUE, POP::NONE}); break;
            };
        }
        return element_vector;
    }
}