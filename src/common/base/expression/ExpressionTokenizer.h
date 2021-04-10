#ifndef SKYGUARD_POLICY_ENGINE_EXPRESSIONTOKENIZER_H_H
#define SKYGUARD_POLICY_ENGINE_EXPRESSIONTOKENIZER_H_H

#include <stack>
#include <vector>
#include <functional>
#include <iostream>
#include <skyguard/policy_engine/expression/BinOpteratorManager.h>

SGPE_BEGIN

typedef std::set<std::string> OperatorTable;
typedef std::stack<OpSignPtr> SignExpressionStack;
typedef std::shared_ptr<SignExpressionStack> SignExpressionStackPtr;
typedef std::vector<OpSignPtr> SignExpressionVector;
typedef std::shared_ptr<SignExpressionVector> SignExpressionVectorPtr;

template <typename T>
class ExpressionTokenizer
{
public:
    ExpressionTokenizer()
    {}

    ExpressionTokenizer(const OperatorTable& optr_tbl)
        : mOptrTable(optr_tbl)
    {}

    void registerOprdCaster(std::function<T(const std::string&)> caster)
    {
        mCaster = caster;
    }

    void setBinOpteratorManager(BinOpteratorManagerPtr<T> optr_manager)
    {
        mOptrManager = optr_manager;
    }

    std::vector<std::string> tokenize(const std::string& exp) const;

    SignExpressionVector generateSignExp(const std::vector<std::string>& exp) const;

private:
    OperatorTable mOptrTable;
    BinOpteratorManagerPtr<T> mOptrManager;
    std::function<T(const std::string&)> mCaster;
};
template<typename T>
    using ExpressionTokenizerPtr = std::shared_ptr<ExpressionTokenizer<T>>;

template <typename T>
std::vector<std::string> ExpressionTokenizer<T>::tokenize(const std::string& exp) const
{
    std::string preprocess_exp(exp);
    for (const auto optr : mOptrTable)
        boost::replace_all(preprocess_exp, optr, " " + optr + " ");

    std::vector<std::string> signs;
    std::istringstream iss(preprocess_exp);
    std::string word;
    while (iss >> word)
        signs.push_back(word);

    return signs;
}

template <typename T>
SignExpressionVector ExpressionTokenizer<T>::generateSignExp(const std::vector<std::string>& exp) const
{
    auto is_optr = [&](const std::string& sign) -> bool
    {
        return (mOptrTable.find(sign) != mOptrTable.end()) ? true : false;
    };

    SignExpressionVector sign_exp;
    for (const auto& sign : exp)
    {
        OpSignPtr op_sign;
        if (is_optr(sign))
        {
            op_sign = mOptrManager->getOpterator(sign);
        }
        else
        {
            op_sign = std::make_shared<Operand<T>>(mCaster(sign));
        }

        if (!op_sign)
        {
            std::cout << "sign:" << sign << " cannot be parsed" << std::endl;
            continue;
        }

        sign_exp.push_back(op_sign);
    }

    return sign_exp;
}

SGPE_END
#endif
