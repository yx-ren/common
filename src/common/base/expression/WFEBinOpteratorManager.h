#ifndef SKYGUARD_POLICY_ENGINE_WFE_BIN_OPTERATOR_MANAGER_H_H
#define SKYGUARD_POLICY_ENGINE_WFE_BIN_OPTERATOR_MANAGER_H_H

#include <vector>
#include <algorithm>
#include <mutex>
#include <skyguard/policy_engine/expression/BinOpterator.h>

SGPE_BEGIN

class WFEBinOpteratorManager : public BinOpteratorManager<bool>
{
public:
    static std::shared_ptr<WFEBinOpteratorManager> getInstance()
    {
        if (!mInstance)
        {
            std::unique_lock<std::mutex> lock(mLock);
            if (!mInstance)
                mInstance = std::make_shared<WFEBinOpteratorManager>();
        }

        return mInstance;
    }

    static void freeInstance()
    {
        std::unique_lock<std::mutex> lock(mLock);
        if (mInstance)
            mInstance.reset();
    }

    virtual ~WFEBinOpteratorManager()
    {}

    WFEBinOpteratorManager()
        : BinOpteratorManager<bool>()
    {
        std::vector<BinOpteratorPtr<bool>> PreBinOpteratorStore =
        {
            std::make_shared<BinOpterator<bool>>("AND", 1, [](OperandPtr<bool> lhs, OperandPtr<bool> rhs)
                    { return std::make_shared<Operand<bool>>(lhs->getValue() && rhs->getValue()); }),
            std::make_shared<BinOpterator<bool>>("OR", 1, [](OperandPtr<bool> lhs, OperandPtr<bool> rhs)
                    { return std::make_shared<Operand<bool>>(lhs->getValue() || rhs->getValue()); }),
            std::make_shared<BinOpterator<bool>>("(", 1, nullptr),
            std::make_shared<BinOpterator<bool>>(")", 1, nullptr),
        };
        setBinOpterators(PreBinOpteratorStore);
    }

protected:
    static std::mutex mLock;
    static std::shared_ptr<WFEBinOpteratorManager> mInstance;
};
typedef std::shared_ptr<WFEBinOpteratorManager> WFEBinOpteratorManagerPtr;

std::mutex WFEBinOpteratorManager::mLock;
std::shared_ptr<WFEBinOpteratorManager> WFEBinOpteratorManager::mInstance;


SGPE_END
#endif
