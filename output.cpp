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
Value("--- SYSTEM BOOTING ---").print();
std::cout << std::endl;
Value("Initializing Titan Test Sequence...").print();
std::cout << std::endl;
Value("[PHASE 1] Array Sorting...").print();
std::cout << std::endl;
getVar("data")[(int)(Value(0).getVal())] = Value(100);
getVar("data")[(int)(Value(1).getVal())] = Value(12.000000);
getVar("data")[(int)(Value(2).getVal())] = Value(45.200000);
getVar("data")[(int)(Value(3).getVal())] = Value(5.100000);
getVar("data")[(int)(Value(4).getVal())] = Value(23.800000);
Value("Unsorted Data Loaded. Commencing Sort...").print();
std::cout << std::endl;
setVar("i", Value(0));
while (getVar("i").getVal() <= Value(4).getVal()) {
setVar("j", Value(0));
while (getVar("j").getVal() <= Value(4).getVal()) {
if ((getVar("data")[(int)(getVar("i").getVal())].getVal() < getVar("data")[(int)(getVar("j").getVal())].getVal())) {
setVar("temp", getVar("data")[(int)(getVar("i").getVal())]);
getVar("data")[(int)(getVar("i").getVal())] = getVar("data")[(int)(getVar("j").getVal())];
getVar("data")[(int)(getVar("j").getVal())] = getVar("temp");
}
setVar("j", Value(getVar("j").getVal() + 1));
}
setVar("i", Value(getVar("i").getVal() + 1));
}
Value("Sort Complete. Lowest Value:").print();
std::cout << std::endl;
getVar("data")[(int)(Value(0).getVal())].print();
std::cout << std::endl;
Value("Highest Value:").print();
std::cout << std::endl;
getVar("data")[(int)(Value(4).getVal())].print();
std::cout << std::endl;
Value("[PHASE 2] 3x3 Matrix Multiplication...").print();
std::cout << std::endl;
setVar("i", Value(0));
while (getVar("i").getVal() <= Value(2).getVal()) {
setVar("j", Value(0));
while (getVar("j").getVal() <= Value(2).getVal()) {
getVar("matA")[(int)(getVar("i").getVal())][(int)(getVar("j").getVal())] = Value(getVar("i").getVal() + getVar("j").getVal());
getVar("matB")[(int)(getVar("i").getVal())][(int)(getVar("j").getVal())] = Value(getVar("i").getVal() * getVar("j").getVal());
getVar("resultMat")[(int)(getVar("i").getVal())][(int)(getVar("j").getVal())] = Value(0);
setVar("j", Value(getVar("j").getVal() + 1));
}
setVar("i", Value(getVar("i").getVal() + 1));
}
setVar("i", Value(0));
while (getVar("i").getVal() <= Value(2).getVal()) {
setVar("j", Value(0));
while (getVar("j").getVal() <= Value(2).getVal()) {
setVar("k", Value(0));
while (getVar("k").getVal() <= Value(2).getVal()) {
setVar("product", Value(getVar("matA")[(int)(getVar("i").getVal())][(int)(getVar("k").getVal())].getVal() * getVar("matB")[(int)(getVar("k").getVal())][(int)(getVar("j").getVal())].getVal()));
getVar("resultMat")[(int)(getVar("i").getVal())][(int)(getVar("j").getVal())] = Value(getVar("resultMat")[(int)(getVar("i").getVal())][(int)(getVar("j").getVal())].getVal() + getVar("product").getVal());
setVar("k", Value(getVar("k").getVal() + 1));
}
setVar("j", Value(getVar("j").getVal() + 1));
}
setVar("i", Value(getVar("i").getVal() + 1));
}
Value("Matrix Processed. Check Result Matrix Index [2][2]:").print();
std::cout << std::endl;
getVar("resultMat")[(int)(Value(2).getVal())][(int)(Value(2).getVal())].print();
std::cout << std::endl;
Value("[PHASE 3] OOP Grid Traversal...").print();
std::cout << std::endl;
setVar("rover", Value(new std::map<std::string, Value>()));
(*(getVar("rover").properties))["x"] = Value(0);
(*(getVar("rover").properties))["y"] = Value(0);
(*(getVar("rover").properties))["battery"] = Value(100);
(*(getVar("rover").properties))["isActive"] = Value(1);
setVar("target_x", Value(4));
setVar("target_y", Value(3));
Value("Rover Deployed. Target acquired at [4, 3]").print();
std::cout << std::endl;
while (((*(getVar("rover").properties))["isActive"].getVal() == Value(1).getVal())) {
if (((*(getVar("rover").properties))["x"].getVal() == getVar("target_x").getVal())) {
if (((*(getVar("rover").properties))["y"].getVal() == getVar("target_y").getVal())) {
Value("DESTINATION REACHED.").print();
std::cout << std::endl;
(*(getVar("rover").properties))["isActive"] = Value(0);
break;
}
}
if (((*(getVar("rover").properties))["battery"].getVal() < Value(10).getVal())) {
Value("CRITICAL BATTERY. HALTING.").print();
std::cout << std::endl;
(*(getVar("rover").properties))["isActive"] = Value(0);
break;
}
if (((*(getVar("rover").properties))["x"].getVal() < getVar("target_x").getVal())) {
(*(getVar("rover").properties))["x"] = Value((*(getVar("rover").properties))["x"].getVal() + Value(1).getVal());
(*(getVar("rover").properties))["battery"] = Value((*(getVar("rover").properties))["battery"].getVal() - Value(8).getVal());
Value("Moved X. Current X:").print();
std::cout << std::endl;
(*(getVar("rover").properties))["x"].print();
std::cout << std::endl;
}
if (((*(getVar("rover").properties))["y"].getVal() < getVar("target_y").getVal())) {
(*(getVar("rover").properties))["y"] = Value((*(getVar("rover").properties))["y"].getVal() + Value(1).getVal());
(*(getVar("rover").properties))["battery"] = Value((*(getVar("rover").properties))["battery"].getVal() - Value(8).getVal());
Value("Moved Y. Current Y:").print();
std::cout << std::endl;
(*(getVar("rover").properties))["y"].print();
std::cout << std::endl;
}
}
Value("Final Battery Level:").print();
std::cout << std::endl;
(*(getVar("rover").properties))["battery"].print();
std::cout << std::endl;
Value("--- TITAN SEQUENCE COMPLETE ---").print();
std::cout << std::endl;
    return 0;
}
