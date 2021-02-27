#include <algorithm>
#include <string>
#include <memory>
#include <stack>
#include <vector>
#include <iostream>
#include <sstream>
#include <functional>

// ********************  OpSign implemetation begin ******************** //

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

// ********************  OpSign implemetation end ******************** //

// ********************  Operand implemetation begin ******************** //

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

template<typename T>
    using BinCalculator = std::function<OperandPtr<T>(OperandPtr<T>, OperandPtr<T>)>;

template<>
std::string Operand<int>::toString() const
{
    return std::to_string(mOperand);
}

template<>
std::string Operand<bool>::toString() const
{
    std::ostringstream oss;
    oss << std::boolalpha << false;
    return oss.str();
}

template<>
std::string Operand<std::string>::toString() const
{
    return mOperand;
}


// ********************  Operand implemetation end ******************** //

// ********************  BinOpterator implemetation begin ******************** //

template<typename T>
class BinOpterator : public OpSign
{
public:
    BinOpterator(const std::string& sign, int priority, BinCalculator<T> calc)
        : OpSign(OPST_OPERATOR, sign)
        , mPriority(priority)
        , mCalc(calc)
    {}

    virtual ~BinOpterator() {}

    OperandPtr<T> calculate(OperandPtr<T> lhs, OperandPtr<T> rhs) { return mCalc(lhs, rhs); }

private:
    int mPriority;
    BinCalculator<T> mCalc;
};
template<typename T>
    using BinOpteratorPtr = std::shared_ptr<BinOpterator<T>>;

// ********************  BinOpterator implemetation end ******************** //

// ********************  BinOpteratorManager implemetation begin ******************** //

template<typename T>
class BinOpteratorManager
{
public:
    BinOpteratorManager()
    {
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
    }

    BinOpteratorPtr<T> getOpterator(const std::string& optr)
    {
        auto ret = std::find_if(mOpterators.begin(), mOpterators.end(),
                [&](const BinOpteratorPtr<T> bin_optr) { return optr == bin_optr->getSign();});
        return ret != mOpterators.end() ? *ret : nullptr;
    }

protected:
    BinOpteratorManager(const BinOpteratorManager&);
    BinOpteratorManager& operator=(const BinOpteratorManager&);

private:
    std::vector<BinOpteratorPtr<T>> mOpterators;
};

// ********************  BinOpteratorManager implemetation end ******************** //

// ********************  PRN implemetation begin ******************** //

#if 0
class BaseExpression
{
public:

private:
    std::string mExp;
};

typedef BaseExpression PrefixExp;
typedef BaseExpression InfixExp;
typedef BaseExpression PostfixExp;

#endif

#if 1
template<typename T>
class PRNParser
{
public:
    PRNParser() {}

    PRNParser(const std::map<std::string>& optr_tbl)
        : mOptrTable(optr_tbl)
    {}

    void setOptrTable(const std::map<std::string>& optr_tbl)
    {
        mOptrTable = optr_tbl;
    }

    const std::map<std::string>& getOptrTable() const
    {
        return mOptrTable;
    }

#if 1
    //void setParser(std::function<std::stack<OpSignPtr>>(const std::string& expression) parser);

    //std::stack<OpSignPtr> parse(const std::string& expression); // infix exp -> postfix exp
    //virtual std::stack<OpSignPtr> parse(const std::string& expression) = 0; // infix exp -> postfix exp
    virtual std::stack<OpSignPtr> parse(const std::string& expression) = 0; // infix exp -> postfix exp
#endif

private:
    std::map<std::string> mOptrTable;
};

template<typename T>
    using PRNParser = std::function<std::stack<OpSignPtr<T>>(const std::string& expression)>;

template<typename T>
class RPNExpression
{
public:
    RPNExpression(const std::string& infix_exp)
        : mInfixExpression(infix_exp)
    {}

    OperandPtr<T> evaluate();

#if 1
    void setParser(PRNParser<T> parser) {mParser = parser};
#endif

private:
    std::string mInfixExpression;
    PRNParser<T> mParser;
};

template<typename T>
OperandPtr<T> RPNExpression<T>::evaluate()
{
    std::stack<OpSignPtr> op_signs = mParser.parse(mInfixExpression);

    std::stack<OperandPtr<T>> operands;
    std::stack<OperandPtr<T>> operators;
    while (!op_signs.empty())
    {
        auto sign = op_signs.top(); op_signs.top();
        if (sign->getType() == OPST_OPERAND)
        {
            operands.push(sign);
        }
        else if (sign->getType() == OPST_OPERATOR)
        {
            if (operands.size() < 2)
                return nullptr;

            auto lhs_oprd = operands.top(); operands.pop();
            auto rhs_oprd = operands.top(); operands.pop();

            auto optr = dynamic_pointer_cast<OperandPtr>(sign);
            if (!optr)
                return nullptr;

            operands.push(optr->calculate(lhs_oprd, rhs_oprd));
        }
    }

    if (operands.size() != 1)
        return nullptr;

    return operands.top();
}
#endif

// ********************  PRN implemetation end ******************** //

