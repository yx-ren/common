#ifndef SKYGUARD_POLICY_ENGINE_PRN_EXPRESSION_H_H
#define SKYGUARD_POLICY_ENGINE_PRN_EXPRESSION_H_H

#include <skyguard/policy_engine/expression/PRNParser.h>
#include <skyguard/base/Logger.h>

using namespace SGBASE_NAMESPACE;

SGPE_BEGIN

template<typename T>
class RPNExpression
{
public:
    OperandPtr<T> evaluate(const std::string& infix_exp);

    void setParser(PRNParserPtr<T> parser) {mParser = parser;}

private:
    PRNParserPtr<T> mParser;
};
template<typename T>
    using RPNExpressionPtr = std::shared_ptr<RPNExpression<T>>;

template<typename T>
OperandPtr<T> RPNExpression<T>::evaluate(const std::string& infix_exp)
{
    SignExpressionStackPtr op_signs_stack = mParser->parse(infix_exp);
    SignExpressionVector op_signs_vector;
    if (op_signs_stack)
    {
        while (!op_signs_stack->empty())
        {
            op_signs_vector.push_back(op_signs_stack->top());
            op_signs_stack->pop();
        }
    }
    std::reverse(op_signs_vector.begin(), op_signs_vector.end());

    std::stack<OperandPtr<T>> operands;
    for (auto& sign : op_signs_vector)
    {
        if (sign->getType() == OpSign::OPST_OPERAND)
        {
            auto oprd = std::dynamic_pointer_cast<Operand<T>>(sign);
            if (!oprd)
            {
                std::cout << "OpSignPtr cast to Operand failed, sign:" << sign->getSign() << std::endl;
                return nullptr;
            }
            operands.push(oprd);
        }
        else if (sign->getType() == OpSign::OPST_OPERATOR)
        {
            if (operands.size() < 2)
            {
                std::cout << "operands not match" << std::endl;
                return nullptr;
            }

            auto lhs_oprd = operands.top(); operands.pop();
            auto rhs_oprd = operands.top(); operands.pop();

            auto optr = std::dynamic_pointer_cast<BinOpterator<T>>(sign);
            if (!optr)
            {
                std::cout << "OpSignPtr cast to BinOpteratorPtr failed, sign:" << sign->getSign() << std::endl;
                return nullptr;
            }

            operands.push(optr->calculate(lhs_oprd, rhs_oprd));
        }
    }

    if (operands.size() != 1)
        return nullptr;

    return operands.top();
}

SGPE_END
#endif
