#ifndef SKYGUARD_POLICY_ENGINE_BIN_OPERATOR_H_H
#define SKYGUARD_POLICY_ENGINE_BIN_OPERATOR_H_H

#include <skyguard/policy_engine/expression/Operand.h>

SGPE_BEGIN

typedef int OptrPriority;

template<typename T>
    using BinCalculator = std::function<OperandPtr<T>(OperandPtr<T>, OperandPtr<T>)>;

template<typename T>
class BinOpterator : public OpSign
{
public:
    BinOpterator(const std::string& sign)
        : OpSign(OPST_OPERATOR, sign)
    {}

    BinOpterator(const std::string& sign, int priority)
        : OpSign(OPST_OPERATOR, sign)
        , mPriority(priority)
    {}

    BinOpterator(const std::string& sign, int priority, BinCalculator<T> calc)
        : OpSign(OPST_OPERATOR, sign)
        , mPriority(priority)
        , mCalc(calc)
    {}

    virtual ~BinOpterator() {}

    int getPriority() const
    {
        return mPriority;
    }

    OperandPtr<T> calculate(OperandPtr<T> lhs, OperandPtr<T> rhs) { return mCalc(lhs, rhs); }

private:
    OptrPriority mPriority;
    BinCalculator<T> mCalc;
};
template<typename T>
    using BinOpteratorPtr = std::shared_ptr<BinOpterator<T>>;

SGPE_END
#endif
