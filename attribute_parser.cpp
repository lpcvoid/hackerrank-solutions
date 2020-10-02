/*
 * url: https://www.hackerrank.com/challenges/attribute-parser/problem
 * this was a fun one.
 */



#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>
#include <iterator>
#include <sstream>

struct attribute {
    std::string name;
    std::string value;
};
struct node {
    std::string name;
    std::vector<attribute> attributes;
    std::vector<node*> children;
    node* parent;
    bool terminator;

    node(std::string name, node* parent){
        this->name = name;
        this->parent = parent;
        this->terminator = false;
    }
};
enum tokentype {
    tok_angleopen, tok_identifier, tok_attributevalue, tok_angleclose, tok_slash, tok_equals, tok_wavy, tok_dot
};
enum parsestate {
    par_start, par_ident, par_attributename, par_attributevalue, par_end
};
struct token {
    token(tokentype tt, const std::string &value = "") : tt(tt), value(value) {}
    tokentype tt;
    std::string value;
};
std::vector<token> tokenize_expr(std::string str){
    std::vector<token> res;

    int caret = 0;
    std::string memory;
    while (caret < str.size()){
        char input = str.at(caret++);
        switch (input) {
            case '<':{
                res.emplace_back(token(tok_angleopen));
            } break;

            case '>':{
                if (!memory.empty())
                    res.emplace_back(token(tok_identifier, memory));
                res.emplace_back(token(tok_angleclose));
            } break;

            case '"':{
                memory.clear();
                //this is a value, read until next quote
                int valuelen = 0;
                while (str.at(caret++) != '"'){valuelen++;};
                res.emplace_back(token(tok_attributevalue, std::string(str.begin()+caret-valuelen-1, str.begin()+caret-1)));

            } break;

            case '~':{
                if (!memory.empty())
                    res.emplace_back(token(tok_identifier, memory));
                res.emplace_back(token(tok_wavy));
                memory.clear();

            } break;

            case '.':{
                if (!memory.empty())
                    res.emplace_back(token(tok_identifier, memory));

                memory.clear();
                res.emplace_back(token(tok_dot));
            } break;

            case '/':{
                res.emplace_back(token(tok_slash));
            } break;

            case '=':{
                if (!memory.empty())
                    res.emplace_back(tok_identifier, memory);

                res.emplace_back(tok_equals);
                memory.clear();
            } break;

            case ' ':{
                if (!memory.empty())
                    res.emplace_back(tok_identifier, memory);
                memory.clear();
            } break;

            default:{
                memory += input;
            }
        }
    }
    //edge case with query, string can terminate and we must check if there is any data remaining in memory
    if (!memory.empty())
        res.emplace_back(tok_identifier, memory);

    return res;
}

node* parse_node(std::string nodestr){

    node* res = new node("", nullptr);
    auto tokens = tokenize_expr(nodestr);
    std::reverse(tokens.begin(), tokens.end());
    parsestate state = par_start;

    while (true){
        switch (state) {
            case par_start:
                if (tokens.back().tt != tok_angleopen){
                    std::cout << "expression did not start with angle bracket" << std::endl;
                    return nullptr;
                }
                tokens.pop_back();
                if (tokens.back().tt == tok_slash){
                    res->terminator = true;
                    tokens.pop_back();
                }
                state = par_ident;
                break;

            case par_ident:
                if (tokens.back().tt != tok_identifier){
                    std::cout << "expression does not contain identifier" << std::endl;
                    return nullptr;
                }
                res->name = tokens.back().value;
                tokens.pop_back();
                if (tokens.back().tt == tok_identifier)
                    state = par_attributename;
                else {
                    state = par_end;
                }
                break;
            case par_attributename:
                res->attributes.resize(res->attributes.size()+1);
                res->attributes.back().name = tokens.back().value;
                tokens.pop_back();
                if (tokens.back().tt == tok_equals){
                    tokens.pop_back();
                    state = par_attributevalue;
                } else {
                    std::cout << "attribute not delimited by equals sign" << std::endl;
                    return nullptr;
                }
                break;
            case par_attributevalue:
                res->attributes.back().value = tokens.back().value;
                tokens.pop_back();
                if ((tokens.back().tt == tok_angleclose) || (tokens.back().tt == tok_slash)){
                    //this was the last attribute pair, terminate
                    state = par_end;
                } else {
                    state = par_attributename;
                }
                break;
            case par_end:
                return res;
        }
    }
};

std::string eval_query(std::string query, std::vector<node*> trees){
    auto tokens = tokenize_expr(query);
    std::reverse(tokens.begin(), tokens.end());
    parsestate state = par_ident;
    std::vector<node*> current_level = trees;
    node* current_node = nullptr;
    while (true) {
        switch (state) {
            case par_ident: {
                std::string ident = tokens.back().value;
                tokens.pop_back();
                auto f_it = std::find_if(current_level.begin(), current_level.end(), [&](node *n) {
                    return (n->name == ident);
                });

                if (f_it != current_level.end()) {
                    //found identifier on current level
                    //increase current level and advance to next ident or attribute
                    current_node = (*f_it);
                    current_level = (*f_it)->children;
                    if (tokens.back().tt == tok_dot) {
                        tokens.pop_back();
                        state = par_ident;
                    } else if (tokens.back().tt == tok_wavy) {
                        tokens.pop_back();
                        state = par_attributename;
                    }
                } else return "Not found!";
            } break;
            case par_attributename:{
                std::string attribute =  tokens.back().value;
                auto f_it = std::find_if(current_node->attributes.begin(), current_node->attributes.end(), [&](struct attribute a) {
                    return (a.name == attribute);
                });
                if (f_it != current_node->attributes.end()) {
                    return (*f_it).value;
                } else return "Not found!";
            } break;
            case par_end:
                break;
        }
    }
}
int main(){
    std::string input;
    std::getline(std::cin, input);
    std::stringstream ss(input);
    auto startparse = std::istream_iterator<int>{ss};
    auto endparse = std::istream_iterator<int>{};
    std::vector<int> numparams(startparse, endparse);
    std::stack<node*> inputstack;
    std::vector<node*> trees;

    for (int i =0; i < numparams.front(); i++){
        std::getline(std::cin, input);
        node* n = parse_node(input);
        if (n){
            if (n->terminator) {
                if (inputstack.top()->name == n->name)
                    inputstack.pop();
                else
                    std::cout << " element termination order incorrect" << std::endl;
            } else {
                if (!inputstack.empty()){
                    n->parent = inputstack.top();
                    inputstack.top()->children.push_back(n);
                } else {
                    trees.push_back(n);
                }
                inputstack.emplace(n);
            }
        }
    }
    //query
    for (int i =0; i < numparams.back(); i++){
        std::getline(std::cin, input);
        std::cout << eval_query(input, trees) << std::endl;
    }
    return 0;
}