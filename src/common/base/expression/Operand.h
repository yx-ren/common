#ifndef SKYGUARD_POLICY_ENGINE_OPERAND_H_H
#define SKYGUARD_POLICY_ENGINE_OPERAND_H_H

#include <iostream>
#include <sstream>
#include <skyguard/policy_engine/expression/OpSign.h>

SGPE_BEGIN

template<typename T>
class Operand : public OpSign
{
public:
    Operand(T operand)
        : OpSign(OPST_OPERAND, "")
        , mOperand(operand)
    {
        if (mSign.empty())
            mSign = toString();
    }

    virtual ~Operand() {}

    void setValue(T val) { mOperand = val; }

    T getValue() { return mOperand; }
    const T& getValue() const { return mOperand; }

    virtual std::string toString() const;

private:
    T mOperand;
};
template<typename T>
    using OperandPtr = std::shared_ptr<Operand<T>>;

template<>
inline std::string Operand<int>::toString() const
{
    return std::to_string(mOperand);
}

template<>
inline std::string Operand<bool>::toString() const
{
    std::ostringstream oss;
    oss << std::boolalpha << mOperand;
    return oss.str();
}

template<>
inline std::string Operand<std::string>::toString() const
{
    return mOperand;
}

SGPE_END
#endif
