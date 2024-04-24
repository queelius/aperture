#include <string>
#include <map>
#include <vector>

struct expr
{

};

struct env
{
    env * par;
    std::map<std::string, expr*> vals;

    env(std::vector<std::string> parms,
        std::vector<expr*> args,
        env * par = nullptr) :
            par(par)
    {
        for (int i = 0; i < params.size(); ++i) vals[params[i]] = args[i]
    }

    auto & find(std::string x)
    {
        return vals.find(x) == vals.end() ? par->find(x) : this;
    }

    auto & operator[](std::string x) { return vals[x]; }
};

struct symbol : expr
{
    std::string value;
};

struct constant : expr
{
    int x;
};

struct lambda : expr
{
    expr * body;
    std::string params;
    env * local;
};

struct proc
{
    expr * body;
    std::vector<std::string> parms;
    env * local;
    
    proc(std::vector<std::string> parms,
         expr * body,
         env * local) : 
        params(params), body(body), local(local) {}

    auto operator()(std::vector<expr*> args) { return eval(body, env(parms, args, local)) }
};

auto is_sym(expr * x) { return std::dynamic_cast<symbol>(x) != nullptr; }
auto is_const(expr * x) { return std::dynamic_cast<constant>(x) != nullptr; }
auto is_lam(expr * x) { return true; /* use unicode lambda */ }
auto is_if(expr * x) { return true; }
auto is_def(expr * x) { return true; }
auto is_proc(expr * x) { return std::dynamic_cast<proc>(x) != nullptr; }

auto eval(expr * x, env * e)
{
    if      (is_sym(x))   return e->find(x)[x];
    else if (is_const(x)) return x;
    
    auto args = get_args(x);
    auto op = get_op(x);
    if      (is_quot(op)) return args[0];
    else if (is_if(op))   return eval(eval(args[0], env) ? args[1] : args[2], env);
    else if (is_def(op)   return env[args[0]] = eval(args[1], env);
    else if (is_lam(op))  return proc(args[0], args[1], env);
    else
    {
        auto proc = eval(op, env);
        return apply([&env, &proc](arg) { proc(eval(arg, env)) }, args);
    }
}