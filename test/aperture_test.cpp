#include <string>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>
#include <regex>
#include <exception>
#include <optional>

#include "../include/aperture/detail/aperture.hpp"
#include "../include/aperture/detail/pretty_printer.hpp"

void test_define()
{
    using namespace aperture::details;        

    env * global_env = standard_env();

    std::string code = "(define x 10)";
    auto [b,e] = tokenize(code.begin(),code.end());
    exp * res1 = read(b,e);
    std::cout << "\n\n==========================\n\n";
    print(res1);
    std::cout << "\n\n==========================\n\n";
    exp * res2 = eval(res1,global_env);
    print(res2);

    code = "(define plus +)";
    auto [f,l] = tokenize(code.begin(),code.end());
    exp * res3 = read(f,l);
    std::cout << "\n\n==========================\n\n";
    print(res3);
    std::cout << "\n\n==========================\n\n";
    exp * res4 = eval(res3,global_env);
    print(res4);
}

void test_apply()
{
    //eval(new define("z",new integer(20000)),global_env);
    //auto body = cons(new symbol("+"), cons(new symbol("x"), cons(new symbol("z"), NIL)));
    //auto params = cons(new symbol("x"), cons(new symbol("y"), NIL));
    //auto args = cons(new integer(1000), cons(new integer(200), NIL));
    //auto A2 = new lambda(params,body,global_env);
    //auto B2 = apply(A2,args);
}

void test_lambda()
{
    using namespace aperture::details;        

    env * global_env = standard_env();

    std::string code = "((lambda (x y) (+ x y)) 1 2)";
    //std::string code = "(define f (+ ((lambda (x y) (+ x y)) 11 22) 2))";
    //std::string code = "(define x 10)";
    auto [b,e] = tokenize(code.begin(),code.end());
    exp * res2 = read(b,e);
    std::cout << "\n\n==========================\n\n";
    print(res2);
    std::cout << "\n\n==========================\n\n";
    exp * res3 = eval(res2,global_env);
    print(res3);
}

void test_lift()
{
    using namespace aperture::details;        

    env * global_env = standard_env();

    std::string code = "(lambda (x y) (+ x y))";
    auto [b,e] = tokenize(code.begin(),code.end());
    exp * res = read(b,e);
    print(res);
    exp * res2 = eval(res,global_env);
    print(res2);

    //auto f = aperture::lift<exp*,integer*>(res2);
    //std::cout << "A\n";
    //auto n = cons(new integer(1), cons(new integer(4), cons(new integer(2), NIL)));
    //print(n);
    //std::cout << "is_list? " << is_list(n) << "\n";
    //std::cout << "B\n";
    //auto a = f(n);
    //std::cout << "C\n";
    //print(a);
}

void test_env()
{
    //print(global_env->lookup("x"));
}

void test_value_semantics()
{
    //auto global = aperture::standard_env();
//
  //  std::string code = "((lambda (x y) (+ x y)) 1 2)";
    //auto x = aperture::exp(code.begin(),code.end());
    //auto res = aperture::eval(x,global);
    //std::cout << "result: " << aperture::details::value<aperture::details::integer>(res.e) << "\n";
}


int main()
{
    //test_define();
    test_lambda();
    //test_lift();
}
