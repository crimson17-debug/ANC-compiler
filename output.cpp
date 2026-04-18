#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cstdlib>

struct Value {
    int type;
    int num;
    double decimal;
    std::string str;
    std::map<std::string, Value>* properties;
    std::map<int, Value>* arrayElements;
    Value() : type(0), num(0), decimal(0.0), properties(nullptr), arrayElements(nullptr) {}
    Value(int n) : type(0), num(n), decimal(0.0), properties(nullptr), arrayElements(nullptr) {}
    Value(double d) : type(3), num(0), decimal(d), properties(nullptr), arrayElements(nullptr) {}
    Value(std::string s) : type(1), num(0), decimal(0.0), str(s), properties(nullptr), arrayElements(nullptr) {}
    Value(std::map<std::string, Value>* p) : type(2), num(0), decimal(0.0), properties(p), arrayElements(nullptr) {}
    double getVal() {
        if (type == 3) return decimal;
        return (double)num;
    }
    Value& operator[](int index) {
        if (arrayElements == nullptr) { type = 4; arrayElements = new std::map<int, Value>(); }
        return (*arrayElements)[index];
    }
    void print() {
        if (type == 2) std::cout << "[Object Reference]";
        else if (type == 4) std::cout << "[Array Reference]";
        else if (type == 1) std::cout << str;
        else if (type == 3) std::cout << decimal;
        else std::cout << num;
    }
    void readFromInput() {
        std::string inputStr;
        std::getline(std::cin >> std::ws, inputStr);
        char* end;
        long lval = strtol(inputStr.c_str(), &end, 10);
        if (*end == '\0') { type = 0; num = lval; return; }
        double dval = strtod(inputStr.c_str(), &end);
        if (*end == '\0') { type = 3; decimal = dval; return; }
        type = 1; str = inputStr;
    }
};

std::vector<std::map<std::string, Value> > stack;
std::map<std::string, Value> globals;

Value& getVar(std::string name) {
    if (!stack.empty() && stack.back().count(name)) return stack.back()[name];
    return globals[name];
}

void setVar(std::string name, Value val) {
    if (!stack.empty()) stack.back()[name] = val;
    else globals[name] = val;
}

void push_scope() { stack.push_back(std::map<std::string, Value>()); }
void pop_scope() { if (!stack.empty()) stack.pop_back(); }


int main() {
setVar("x", Value(22.400000));
Value((getVar("x").type == 0) ? "int" : (getVar("x").type == 1) ? "string" : (getVar("x").type == 2) ? "object" : (getVar("x").type == 3) ? "float" : "array").print();
std::cout << std::endl;
    return 0;
}
