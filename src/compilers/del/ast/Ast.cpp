#include "Ast.hpp"

namespace DEL
{
    void Assignment::visit(Visitor &visitor)
    {
        visitor.accept(*this);
    }

    void ReturnStmt::visit(Visitor &visitor)
    {
        visitor.accept(*this);
    }

    void Call::visit(Visitor &visitor)
    {
        visitor.accept(*this);
    }
}