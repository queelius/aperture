#include <iostream>
#include <string>
#include <iostream>
#include "types.h"
#include "expression.hpp"
//#include "addition.hpp"
//#include "multiplication.hpp"
//#include "closure.h"
using std::ostream;
using std::string;


int main()
{
    expression e = num{3};
    /*expression add_e = add_expr{num{3},num{2}};    
    expression add_s = add_expr{str{"3"},str{"2"}};
    expression add_e2 = add_expr{add_e,e};
    expression mult_e3 = mult_expr{add_e2,add_e};*/

    //std::cout << e << "\n";
    //std::cout << add_e << "\n";
    //std::cout << add_s << "\n";
    //std::cout << add_e2 << "\n";
    //std::cout << mult_e3 << "\n";

    std::cout << "===\n";

    //closure c;

    //auto res1 = eval(e,c);
    //auto res2 = eval(add_e,c);
    //auto res3 = eval(add_s,c);
    //auto res4 = eval(add_e2,c);
    //auto res5 = eval(mult_e3,c);

    //std::cout << res1 << "\n";
    //std::cout << res2 << "\n";
    //std::cout << res3 << "\n";
    //std::cout << res4 << "\n";
    //std::cout << res5 << "\n";

    return 0;
}