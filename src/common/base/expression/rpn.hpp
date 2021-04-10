#include <algorithm>
#include <memory>
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

template<>
std::string Operand<int>::toString() const
{
    return std::to_string(mOperand);
}

template<>
std::string Operand<bool>::toString() const
{
    std::ostringstream oss;
    oss << std::boolalpha << mOperand;
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

private:
    std::vector<BinOpteratorPtr<T>> mOpterators;
};
template<typename T>
    using BinOpteratorManagerPtr = std::shared_ptr<BinOpteratorManager<T>>;

// ********************  BinOpteratorManager implemetation end ******************** //

// ********************  ExpressionTokenizer begin ******************** //

#if 1

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

// ********************  ExpressionTokenizer end ******************** //

#endif

// ********************  PRN implemetation begin ******************** //

template<typename T>
class PRNParser
{
public:
    PRNParser() {}

    void setTokenizer(ExpressionTokenizer<T> tokenizer)
    {
        mTokenizer = tokenizer;
    }

    SignExpressionStackPtr parse(const std::string& infix_exp)
    {
        std::vector<std::string> signs = mTokenizer.tokenize(infix_exp);
        SignExpressionVector sign_exp_vector = mTokenizer.generateSignExp(signs);
        return infix2Postfix(sign_exp_vector);
    }

protected:
    // infix exp -> postfix exp
    SignExpressionStackPtr infix2Postfix(const SignExpressionVector infix_exp);

private:
    ExpressionTokenizer<T> mTokenizer;
};

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

template<typename T>
class RPNExpression
{
public:
    RPNExpression(const std::string& infix_exp)
        : mInfixExpression(infix_exp)
    {}

    OperandPtr<T> evaluate();

    void setParser(PRNParser<T> parser) {mParser = parser;}

private:
    std::string mInfixExpression;
    PRNParser<T> mParser;
};

template<typename T>
OperandPtr<T> RPNExpression<T>::evaluate()
{
    SignExpressionStackPtr op_signs_stack = mParser.parse(mInfixExpression);
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

// ********************  PRN implemetation end ******************** //

