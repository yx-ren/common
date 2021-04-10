#ifndef SKYGUARD_POLICY_ENGINE_OP_SIGN_H_H
#define SKYGUARD_POLICY_ENGINE_OP_SIGN_H_H

#include <memory>
#include <string>
#include <skyguard/policy_engine/common.h>

SGPE_BEGIN

class OpSign
{
public:
    enum type
    {
        OPST_UNKNOWN,
        OPST_OPERAND,
        OPST_OPERATOR,
    };

    OpSign()
        : mType(OPST_UNKNOWN)
        , mSign("")
    {}

    OpSign(type t, const std::string& sign)
        : mType(t)
        , mSign(sign)
    {}

    virtual ~OpSign() {}

    void setSign(const std::string& sign) { mSign = sign; }

    const std::string& getSign() const { return mSign; }

    void setType(type t) { mType = t; }

    const type& getType() const { return mType; }

protected:
    type mType;
    std::string mSign;
};
typedef std::shared_ptr<OpSign> OpSignPtr;

SGPE_END
#endif
