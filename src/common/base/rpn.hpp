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
    {}

    BinOpterator(const std::string& sign, int priority, BinCalculator<T> calc)
        : OpSign(OPST_OPERATOR, sign)
        , mPriority(priority)
        , mCalc(calc)
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
template<typename T>
    using BinOpteratorManagerPtr = std::shared_ptr<BinOpteratorManager<T>>;

// ********************  BinOpteratorManager implemetation end ******************** //

// ********************  PRN implemetation begin ******************** //

#if 1

typedef std::set<std::string> OperatorTable;

template <typename T>
    using SignExpression = std::stack<OpSignPtr<T>>;

template <typename T>
class ExpressionTokenizer
{
public:
    ExpressionTokenizer(const OperatorTable& optr_tbl)
        : mOptrTable(optr_tbl)
    {}

    void setBinOpteratorManager(BinOpteratorManagerPtr optr_manager)
    {
        mOptrManager = optr_manager;
    }

    std::stack<std::string> tokenize(const std::string& exp) const;

    SignExpression<T> generateSignExp(const std::stack<std::string>& exp) const;

    void registerOprdCaster(std::function<T(const std::string&)> caster)
    {
        mCaster = caster;
    }

private:
    OperatorTable mOptrTable;
    BinOpteratorManagerPtr mOptrManager;
    std::function<T(const std::string&)> mCaster;
};

template <typename T>
std::stack<std::string> ExpressionTokenizer<T>::tokenize(const std::string& exp) const
{
    std::string preprocess_exp(exp);
    for (const auto optr : mOptrTable)
        boost::replace_all(preprocess_exp, optr, " " + optr + " ");

    std::stack<std::string> signs;
    std::istringstream iss(preprocess_exp);
    std::string word;
    while (iss >> word)
        signs.push(word);

    return signs;
}

template <typename T>
SignExpression<T> ExpressionTokenizer<T>::generateSignExp(const std::stack<std::string>& exp) const
{
    auto is_optr = [&mOptrTable](const std::string& sign) -> bool
    {
        return mOptrTable.find(sign) != mOptrTable.end() ? true : false;
    }

    SignExpression<T> sign_exp;
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
    }

    return sign_exp;
}

enum EXPESSION_TYPE
{
    ET_PREFIX,
    ET_INFOX,
    ET_POSTFIX,
};

template<typename T>
struct BaseExpression
{
    std::string mExp;
    EXPESSION_TYPE mExpType;
    SignExpression<T> mSignExp;
};

#endif

template<typename T>
class PRNParser
{
public:
    void setTokenizer(ExpressionTokenizer tokenizer)
    {
        mTokenizer = tokenizer;
    }

    // infix -> infix
    SignExpression<T> parse(const std::string& infix_exp)
    {
        std::stack<std::string> signs = mTokenizer.tokenize(infix_exp);
        return mTokenizer.generateSignExp(signs);
    }

protected:
    // infix exp -> postfix exp
    SignExpression<T> infix2Postfix(const SignExpression<T> infix_exp);

private:
    ExpressionTokenizer mTokenizer;
};

template<typename T>
SignExpression<T> PRNParser::infix2Postfix(const SignExpression<T> infix_exp)
{
    SignExpression<T> sign_exp;
    std::stack<BinOpteratorPtr<T>> operands;

    for (const auto& sign : infix_exp)
    {
        if (sign->getType() == OPST_OPERAND)
        {
            sign_exp.push(sign);
        }
        else if (sign->getType() == OPST_OPERATOR)
        {
            auto optr = dynamic_pointer_cast<BinOpteratorPtr>(sign);
            while (!operators.empty())
            {
                top_optr = operators.top();
                if (optr->getPriority() < operators.top()->getPriority())
                    sign_exp.push(top_optr); operators.pop();
            }

            if (optr->getSign() == "(")
                sign_exp.push(sign);
            else if (optr->getSign() == ")")
            {
                bool left_match = false
                while (!operators.empty())
                {
                    top_optr = operators.top();
                    if (top_optr != "(")
                        sign_exp.push(top_optr); operators.pop();
                    else
                        left_match = true; operators.pop();
                }

                if (!left_match)
                    return nullptr;
            }
        }
    }

    if (!operands.empty())
    {
        if (operands.top()->getSign() == "(" || operands.top()->getSign() == ")")
            return nullptr;

        std::copy(operands.begin(), operands.end(), std::back_inserter(sign_exp));
    }

    return sign_exp;
}


#if 0
template<typename T>
    using PRNParser = std::function<SignExpression<T>(const std::string& expression) >;
#endif

template<typename T>
class RPNExpression
{
public:
    RPNExpression(const std::string& infix_exp)
        : mInfixExpression(infix_exp)
    {}

    OperandPtr<T> evaluate();

    void setParser(PRNParser<T> parser) {mParser = parser};

protected:
    parse();

private:
    std::string mInfixExpression;
    PRNParser<T> mParser;
};

template<typename T>
OperandPtr<T> RPNExpression<T>::evaluate()
{
    SignExpression<T> op_signs = mParser.parse(mInfixExpression);

    std::stack<OperandPtr<T>> operands;
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

            auto optr = dynamic_pointer_cast<BinOpteratorPtr>(sign);
            if (!optr)
                return nullptr;

            operands.push(optr->calculate(lhs_oprd, rhs_oprd));
        }
    }

    if (operands.size() != 1)
        return nullptr;

    return operands.top();
}

// ********************  PRN implemetation end ******************** //

