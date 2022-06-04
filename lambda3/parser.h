#include <regex>
#include <map>
#include <string>
#include <string_view>
#include <variant>

using std::string;
using std::string_view;
using std::pair;
using std::regex;
using std::map;
using std::variant;
using std::vector;

struct token_rules
{
    // a tokenizer is part of the lexical analysis stage. it doesn't determine
    // whether a string conforms to the syntax of the language (which is done
    // the syntactic analysis stage, or parser).
    //
    // a tokenizers map strings (character sequences) to sequences of tokens,
    // which are slightly higher-level representations of strings. a token is
    // conceptually just a value that groups characters in a string and a
    // corresponding type that describes what the value is.
    map<string,regex> spec;

    void add(string name, string expression)
    {
        add(name,regex(expression));
    }

    void add(string name, regex expression)
    {
        spec[name] =  expression;
    }
};



struct object
{
   string key;
   vector<pair<string, ast>> children;
}

// takes a language syntax and converts it to an AST (S-expression)
// we build the AST interpreter elsewhere.
//
// we also build an AST transformer that takes the AST and maps it to
// a different AST 
struct parser
{
    auto operator()(string_view str)
    {

    }

    // main entry point
    //
    // program
    //     : numeric_literal
    //     ;
    auto program()
    {
        return numeric_literal();
    }

    // numeric_literal
    //     : NUMBER
    //     ;
    auto numeric_literal()
    {
        return std::make_pair("numeric_literal",number)
    }
};