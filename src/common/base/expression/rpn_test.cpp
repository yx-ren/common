#include <iostream>
#include <vector>
#include <memory>

//#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/tokenizer.hpp>
#include <atomic>

#include <boost/filesystem.hpp>

#include <boost/version.hpp>

#include <test/rpn/rpn.hpp>

void test_rpn()
{
    OperandPtr<std::string> str_oprd(std::make_shared<Operand<std::string>>("3str3"));
    std::cout << str_oprd->getValue() << std::endl;

    OperandPtr<int> lhs_oprd(std::make_shared<Operand<int>>(3));
    OperandPtr<int> rhs_oprd(std::make_shared<Operand<int>>(3));

    BinOpteratorManager<int> optr_manager;
    BinOpteratorPtr<int> optr1 = optr_manager.getOpterator("+");
    BinOpteratorPtr<int> optr2 = optr_manager.getOpterator("jia");
    BinOpteratorPtr<int> optr3 = optr_manager.getOpterator("加");

    OperandPtr<int> oprd_1, oprd_2, oprd_3; 
    if (optr1)
    {
        oprd_1 = optr1->calculate(lhs_oprd, rhs_oprd);
        if (oprd_1)
            std::cout << oprd_1->getValue() << std::endl;
    }

    if (optr2)
    {
        oprd_2 = optr2->calculate(oprd_1, oprd_1);
        if (oprd_2)
            std::cout << oprd_2->getValue() << std::endl;
    }

    if (optr3)
    {
        oprd_3 = optr3->calculate(oprd_1, oprd_2);
        if (oprd_3)
            std::cout << oprd_3->getValue() << std::endl;
    }
}


int main(int argc, char* argv[])
{
    //std::string exp = "( true  OR  false  ) AND  true  OR  (true  OR  false AND  true OR true)  ";
    std::string exp = "( true  OR  false  ) AND  true  OR  (true  OR  false AND  (true OR true))";
    std::cout << "origin infix exp:\n" << exp << std::endl;

    {
        // init bin operator manager
        BinOpteratorManagerPtr<bool> optr_manager(std::make_shared<BinOpteratorManager<bool>>());
        std::vector<BinOpteratorPtr<bool>> PreBinOpteratorStore =
        {
            std::make_shared<BinOpterator<bool>>("AND", 1, [](OperandPtr<bool> lhs, OperandPtr<bool> rhs)
                    { return std::make_shared<Operand<bool>>(lhs->getValue() && rhs->getValue()); }),
            std::make_shared<BinOpterator<bool>>("OR", 1, [](OperandPtr<bool> lhs, OperandPtr<bool> rhs)
                    { return std::make_shared<Operand<bool>>(lhs->getValue() || rhs->getValue()); }),
            std::make_shared<BinOpterator<bool>>("(", 1, nullptr),
            std::make_shared<BinOpterator<bool>>(")", 1, nullptr),
        };
        optr_manager->setBinOpterators(PreBinOpteratorStore);

        // init tokenizer
        ExpressionTokenizer<bool> ExpressionTokenizer(OperatorTable({"AND", "OR", "(", ")"}));
        ExpressionTokenizer.setBinOpteratorManager(optr_manager);

        auto bool_caster = [](const std::string& bool_sign) -> bool
        {
            return ((bool_sign == "true") ? true : false);
        };
        ExpressionTokenizer.registerOprdCaster(bool_caster);

        // dump token result
        std::vector<std::string> signs = ExpressionTokenizer.tokenize(exp);
        std::cout << "dump the tokens:" << std::endl;
        for (const auto sign : signs)
            std::cout << sign << " ";
        std::cout << std::endl;

        // dump sign result
        std::cout << "dump the infix sign expression:" << std::endl;
        SignExpressionVector sign_exp_vector = ExpressionTokenizer.generateSignExp(signs);
        for (const auto& sign : sign_exp_vector)
        {
#if 0
            std::cout << "sign:" << sign->getSign()
                << ", type:" << sign->getType()
                << " ";
                //<< std::endl;
#else
            std::cout << sign->getSign() << " ";
#endif
        }
        std::cout << std::endl;

        // init parser
        PRNParser<bool> rpn_parser;
        rpn_parser.setTokenizer(ExpressionTokenizer);

        // test rpn parser
        auto post_fix = rpn_parser.parse(exp);
        SignExpressionVector post_fix_exp;
        if (post_fix)
        {
            while (!post_fix->empty())
            {
                post_fix_exp.push_back(post_fix->top());
                post_fix->pop();
            }
        }

        std::cout << "dump the postfix sign expression:" << std::endl;
        for (auto sign = post_fix_exp.rbegin(); sign != post_fix_exp.rend(); sign++)
        {
#if 0
            std::cout << "sign:" << (*sign)->getSign()
                << ", type:" << (*sign)->getType()
                << " ";
#else
            std::cout << (*sign)->getSign() << " ";
#endif
        }
        std::cout << std::endl;

        // init RPNExpression
        RPNExpression<bool> rpn_exp(exp);
        rpn_exp.setParser(rpn_parser);

        auto res = rpn_exp.evaluate();
        if (res)
        {
            std::cout << res->toString() << std::endl;
        }

        return 0;
    }

    return 0;
}
