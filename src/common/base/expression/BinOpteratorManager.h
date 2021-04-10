#ifndef SKYGUARD_POLICY_ENGINE_BIN_OPTERATOR_MANAGER_H_H
#define SKYGUARD_POLICY_ENGINE_BIN_OPTERATOR_MANAGER_H_H

#include <vector>
#include <algorithm>
#include <skyguard/policy_engine/expression/BinOpterator.h>

SGPE_BEGIN

template<typename T>
class BinOpteratorManager
{
public:
    BinOpteratorManager()
    {
#if 0
        std::vector<BinOpteratorPtr<T>> PreBinOpteratorStore =
        {
            std::make_shared<BinOpterator<T>>("+", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() + rhs->getValue()); }),
            std::make_shared<BinOpterator<T>>("-", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() - rhs->getValue()); }),
            std::make_shared<BinOpterator<T>>("*", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() * rhs->getValue()); }),
            std::make_shared<BinOpterator<T>>("/", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() / rhs->getValue()); }),

            std::make_shared<BinOpterator<T>>("&&", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() && rhs->getValue()); }),
            std::make_shared<BinOpterator<T>>("||", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() || rhs->getValue()); }),
            std::make_shared<BinOpterator<T>>("AND", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() && rhs->getValue()); }),
            std::make_shared<BinOpterator<T>>("OR", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() || rhs->getValue()); }),

            std::make_shared<BinOpterator<T>>("加", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() + rhs->getValue()); }),

            std::make_shared<BinOpterator<T>>("jia", 1, [](OperandPtr<T> lhs, OperandPtr<T> rhs)
                    { return std::make_shared<Operand<T>>(lhs->getValue() + rhs->getValue()); }),
        };

        mOpterators.swap(PreBinOpteratorStore);
#endif
    }

    virtual ~BinOpteratorManager()
    {}

    BinOpteratorPtr<T> getOpterator(const std::string& optr)
    {
        auto ret = std::find_if(mOpterators.begin(), mOpterators.end(),
                [&](const BinOpteratorPtr<T> bin_optr) { return optr == bin_optr->getSign();});
        return ret != mOpterators.end() ? *ret : nullptr;
    }

    void setBinOpterators(const std::vector<BinOpteratorPtr<T>>& optrs)
    {
        mOpterators = optrs;
    }

protected:
    BinOpteratorManager(const BinOpteratorManager&);
    BinOpteratorManager& operator=(const BinOpteratorManager&);

protected:
    std::vector<BinOpteratorPtr<T>> mOpterators;
};
template<typename T>
    using BinOpteratorManagerPtr = std::shared_ptr<BinOpteratorManager<T>>;

SGPE_END
#endif
