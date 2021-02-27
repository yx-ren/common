#include <iostream>
#include <vector>
#include <memory>

#include <test/rpn.hpp>

class Foo
{
public:
    Foo() : mFoo(0) {}

    Foo(int n) : mFoo(n) {}

    virtual ~Foo() {}

    virtual void dump()
    {
        std::cout << mFoo << std::endl;
    }

private:
    int mFoo;
};
typedef std::shared_ptr<Foo> FooPtr;

template<typename T>
class Bar : public Foo
{
public:
    Bar(T val) : mBar(val) {}

    virtual ~Bar() {}

    virtual void dump()
    {
        Foo::dump();
        std::cout << mBar << std::endl;
    }

    using BarPtr_1 = std::shared_ptr<Bar<T>>;

private:
    T mBar;
};

template<typename T>
using BarPtr = std::shared_ptr<Bar<T>>;

typedef std::shared_ptr<Bar<std::string>> BarPtr1;

template<>
Bar<int>::Bar(int n)
    : Foo(n)
{}

template<typename T>
using vectorPtr = std::shared_ptr<std::vector<T>>;

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
    {
#if 0
        FooPtr f1 = std::make_shared<Bar<std::string>>("12345");
        f1->dump();

        return 0;
#endif
    }

    {
        test_rpn();
        return 0;
    }

    {
#if 1
        Bar<std::string>::BarPtr_1 b3(new Bar<std::string>("12345"));
        b3->dump();
        return 0;
#endif

        vectorPtr<int> vec = std::make_shared<std::vector<int>>(std::initializer_list<int>({1, 2, 3, 4, 5}));
        for (const auto it : *vec)
            std::cout << it << std::endl;

        return 0;
    }

    {
        BarPtr1 b1(new Bar<std::string>("123"));
        b1->dump();

        BarPtr<std::string> b2(new Bar<std::string>("12345"));
        b2->dump();

        return 0;
    }

    std::cout << "hello word" << std::endl;

    Foo f1(1);
    Bar<int> b1(2);
    Bar<std::string> b2(std::string("3"));

    f1.dump();
    b1.dump();
    b2.dump();


    return 0;
}
