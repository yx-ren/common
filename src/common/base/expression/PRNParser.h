#ifndef SKYGUARD_POLICY_ENGINE_PRN_PARSER_H_H
#define SKYGUARD_POLICY_ENGINE_PRN_PARSER_H_H

#include <skyguard/policy_engine/expression/ExpressionTokenizer.h>

SGPE_BEGIN

template<typename T>
class PRNParser
{
public:
    PRNParser() {}

    void setTokenizer(ExpressionTokenizerPtr<T> tokenizer)
    {
        mTokenizer = tokenizer;
    }

    SignExpressionStackPtr parse(const std::string& infix_exp)
    {
        std::vector<std::string> signs = mTokenizer->tokenize(infix_exp);
        SignExpressionVector sign_exp_vector = mTokenizer->generateSignExp(signs);
        return infix2Postfix(sign_exp_vector);
    }

protected:
    // infix exp -> postfix exp
    SignExpressionStackPtr infix2Postfix(const SignExpressionVector infix_exp);

private:
    ExpressionTokenizerPtr<T> mTokenizer;
};
template<typename T>
    using PRNParserPtr = std::shared_ptr<PRNParser<T>>;

template<typename T>
SignExpressionStackPtr PRNParser<T>::infix2Postfix(const SignExpressionVector infix_exp)
{
    SignExpressionStackPtr sign_exp = std::make_shared<SignExpressionStack>();
    std::stack<BinOpteratorPtr<T>> operators;

    for (const auto& sign : infix_exp)
    {
        if (sign->getType() == OpSign::OPST_OPERAND)
        {
            sign_exp->push(sign);
        }
        else if (sign->getType() == OpSign::OPST_OPERATOR)
        {
            auto optr = std::dynamic_pointer_cast<BinOpterator<T>>(sign);
            if (!optr)
            {
                std::cout << "BinOpteratorPtr cast to OpSignPtr failed, sign:" << sign->getSign() << std::endl;
                return nullptr;
            }

            if (optr->getSign() == "(")
                operators.push(optr);
            else if (optr->getSign() == ")")
            {
                bool left_match = false;
                while (!operators.empty())
                {
                    auto top_optr = operators.top();
                    if (top_optr->getSign() != "(")
                    {
                        sign_exp->push(top_optr); operators.pop();
                    }
                    else
                    {
                        left_match = true; operators.pop();
                        break;
                    }
                }

#if 1
                if (!left_match)
                {
                    std::cout << "'(' not match" << std::endl;
                    return nullptr;
                }
#endif
            }
            else
            {
                while (!operators.empty())
                {
                    auto top_optr = operators.top();
                    if (top_optr->getSign() == "(")
                        break;
                    else
                    {
                        if (optr->getPriority() <= top_optr->getPriority())
                            sign_exp->push(top_optr); operators.pop();
                    }
                }
                operators.push(optr);
            }
        }
    }

    if (!operators.empty())
    {
        if (operators.top()->getSign() == "(")
        {
            std::cout << "'(' or ')' not match" << std::endl;
            return nullptr;
        }

        while (!operators.empty())
        {
            sign_exp->push(operators.top());
            operators.pop();
        }
    }

    return sign_exp;
}

SGPE_END
#endif
