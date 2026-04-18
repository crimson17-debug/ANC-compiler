#ifndef AST_H
#define AST_H

#include <iostream>
#include <string>
#include <vector>

// --- 3AC Global Counters ---
static int tempVarCounter = 0;
static int labelCounter = 0;

inline std::string newTemp() {
    return "t" + std::to_string(++tempVarCounter);
}

inline std::string newLabel() {
    return "L" + std::to_string(++labelCounter);
}

// --- Tree Formatting Helper ---
inline std::string getIndent(int depth) {
    if (depth == 0) return "";
    std::string ind = "";
    for (int i = 0; i < depth - 1; ++i) ind += "|   ";
    ind += "|-- ";
    return ind;
}

// --- Base AST Node ---
class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual std::string compile() = 0; 
    
    // 3AC Generation Methods
    virtual std::string gen3AC() { return ""; }
    virtual std::string getResult() { return ""; } 
    
    // Syntax Tree Visualization Method
    virtual std::string getSyntaxTree(int depth = 0) { return getIndent(depth) + "UnknownNode\n"; }
};

// --- Literal Nodes ---
class NumberNode : public ASTNode {
    int val;
public:
    NumberNode(int v) : val(v) {}
    std::string compile() override { return "Value(" + std::to_string(val) + ")"; }
    std::string getResult() override { return std::to_string(val); }
    std::string getSyntaxTree(int depth = 0) override { return getIndent(depth) + "Number (" + std::to_string(val) + ")\n"; }
};

class FloatNode : public ASTNode {
    double val;
public:
    FloatNode(double v) : val(v) {}
    std::string compile() override { return "Value(" + std::to_string(val) + ")"; }
    std::string getResult() override { return std::to_string(val); }
    std::string getSyntaxTree(int depth = 0) override { return getIndent(depth) + "Float (" + std::to_string(val) + ")\n"; }
};

class StringNode : public ASTNode {
    std::string val;
public:
    StringNode(char* v) : val(v) {}
    std::string compile() override { return "Value(\"" + val + "\")"; }
    std::string getResult() override { return "\"" + val + "\""; }
    std::string getSyntaxTree(int depth = 0) override { return getIndent(depth) + "String (\"" + val + "\")\n"; }
};

class VariableNode : public ASTNode {
    std::string name;
public:
    VariableNode(char* n) : name(n) {}
    std::string compile() override { return "getVar(\"" + name + "\")"; }
    std::string getResult() override { return name; }
    std::string getSyntaxTree(int depth = 0) override { return getIndent(depth) + "Variable (" + name + ")\n"; }
};

// --- Expression Nodes ---
class BinaryOpNode : public ASTNode {
    char op;
    ASTNode *left, *right;
    std::string resultTemp;
public:
    BinaryOpNode(char o, ASTNode* l, ASTNode* r) : op(o), left(l), right(r) {
        resultTemp = newTemp();
    }
    std::string compile() override { 
        if (op == '<' || op == '>' || op == '=') {
            std::string opStr = (op == '=') ? "==" : std::string(1, op);
            return "(" + left->compile() + ".getVal() " + opStr + " " + right->compile() + ".getVal())";
        }
        return "Value(" + left->compile() + ".getVal() " + op + " " + right->compile() + ".getVal())";
    }
    
    std::string gen3AC() override {
        std::string code = left->gen3AC() + right->gen3AC();
        code += resultTemp + " = " + left->getResult() + " " + op + " " + right->getResult() + "\n";
        return code;
    }
    std::string getResult() override { return resultTemp; }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "BinaryOp (" + op + ")\n";
        res += left->getSyntaxTree(depth + 1);
        res += right->getSyntaxTree(depth + 1);
        return res;
    }
};

// --- Assignment & I/O Nodes ---
class AssignmentNode : public ASTNode {
    std::string varName;
    ASTNode* expr;
public:
    AssignmentNode(char* name, ASTNode* e) : varName(name), expr(e) {}
    std::string compile() override { 
        return "setVar(\"" + varName + "\", " + expr->compile() + ");\n"; 
    }
    
    std::string gen3AC() override {
        std::string code = expr->gen3AC();
        code += varName + " = " + expr->getResult() + "\n";
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "Assignment (" + varName + ")\n";
        res += expr->getSyntaxTree(depth + 1);
        return res;
    }
};

class PrintNode : public ASTNode {
    ASTNode* expr;
public:
    PrintNode(ASTNode* e) : expr(e) {}
    std::string compile() override { 
        return expr->compile() + ".print();\nstd::cout << std::endl;\n"; 
    }
    
    std::string gen3AC() override {
        std::string code = expr->gen3AC();
        code += "print " + expr->getResult() + "\n";
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "Print\n";
        res += expr->getSyntaxTree(depth + 1);
        return res;
    }
};

class InputNode : public ASTNode {
    ASTNode* expr; 
public:
    InputNode(ASTNode* e) : expr(e) {}
    std::string compile() override { 
        return expr->compile() + ".readFromInput();\n"; 
    }
    
    std::string gen3AC() override {
        return "input " + expr->getResult() + "\n";
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "Input\n";
        res += expr->getSyntaxTree(depth + 1);
        return res;
    }
};

// --- Array & Object Nodes ---
class ArrayAccessNode : public ASTNode {
    ASTNode *arrayExpr, *indexExpr;
    std::string resultTemp;
public:
    ArrayAccessNode(ASTNode* arr, ASTNode* idx) : arrayExpr(arr), indexExpr(idx) { resultTemp = newTemp(); }
    std::string compile() override { 
        return arrayExpr->compile() + "[(int)(" + indexExpr->compile() + ".getVal())]"; 
    }
    
    std::string gen3AC() override {
        std::string code = arrayExpr->gen3AC() + indexExpr->gen3AC();
        code += resultTemp + " = " + arrayExpr->getResult() + "[" + indexExpr->getResult() + "]\n";
        return code;
    }
    std::string getResult() override { return resultTemp; }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "ArrayAccess\n";
        res += arrayExpr->getSyntaxTree(depth + 1);
        res += indexExpr->getSyntaxTree(depth + 1);
        return res;
    }
};

class ArrayAssignmentNode : public ASTNode {
    ASTNode *arrayExpr, *indexExpr, *valueExpr;
public:
    ArrayAssignmentNode(ASTNode* arr, ASTNode* idx, ASTNode* val) : arrayExpr(arr), indexExpr(idx), valueExpr(val) {}
    std::string compile() override { 
        return arrayExpr->compile() + "[(int)(" + indexExpr->compile() + ".getVal())] = " + valueExpr->compile() + ";\n"; 
    }
    
    std::string gen3AC() override {
        std::string code = arrayExpr->gen3AC() + indexExpr->gen3AC() + valueExpr->gen3AC();
        code += arrayExpr->getResult() + "[" + indexExpr->getResult() + "] = " + valueExpr->getResult() + "\n";
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "ArrayAssignment\n";
        res += arrayExpr->getSyntaxTree(depth + 1);
        res += indexExpr->getSyntaxTree(depth + 1);
        res += valueExpr->getSyntaxTree(depth + 1);
        return res;
    }
};

class PropertyAccessNode : public ASTNode {
    ASTNode *objExpr;
    std::string propName;
    std::string resultTemp;
public:
    PropertyAccessNode(ASTNode* obj, char* prop) : objExpr(obj), propName(prop) { resultTemp = newTemp(); }
    std::string compile() override { 
        return "(*(" + objExpr->compile() + ".properties))[\"" + propName + "\"]"; 
    }
    
    std::string gen3AC() override {
        std::string code = objExpr->gen3AC();
        code += resultTemp + " = " + objExpr->getResult() + "." + propName + "\n";
        return code;
    }
    std::string getResult() override { return resultTemp; }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "PropertyAccess (" + propName + ")\n";
        res += objExpr->getSyntaxTree(depth + 1);
        return res;
    }
};

class PropertyAssignmentNode : public ASTNode {
    ASTNode *objExpr;
    std::string propName;
    ASTNode* valueExpr;
public:
    PropertyAssignmentNode(ASTNode* obj, char* prop, ASTNode* val) : objExpr(obj), propName(prop), valueExpr(val) {}
    std::string compile() override { 
        return "(*(" + objExpr->compile() + ".properties))[\"" + propName + "\"] = " + valueExpr->compile() + ";\n"; 
    }
    
    std::string gen3AC() override {
        std::string code = objExpr->gen3AC() + valueExpr->gen3AC();
        code += objExpr->getResult() + "." + propName + " = " + valueExpr->getResult() + "\n";
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "PropertyAssignment (" + propName + ")\n";
        res += objExpr->getSyntaxTree(depth + 1);
        res += valueExpr->getSyntaxTree(depth + 1);
        return res;
    }
};

class NewNode : public ASTNode {
    std::string className;
    std::string resultTemp;
public:
    NewNode(char* cls) : className(cls) { resultTemp = newTemp(); }
    std::string compile() override { 
        return "Value(new std::map<std::string, Value>())"; 
    }
    
    std::string gen3AC() override {
        return resultTemp + " = new " + className + "\n";
    }
    std::string getResult() override { return resultTemp; }
    
    std::string getSyntaxTree(int depth = 0) override {
        return getIndent(depth) + "New (" + className + ")\n";
    }
};

// --- Control Flow Nodes ---
class BlockNode : public ASTNode {
    std::vector<ASTNode*> statements;
public:
    void addStatement(ASTNode* stmt) { statements.push_back(stmt); }
    std::string compile() override { 
        std::string code = "";
        for (auto stmt : statements) code += stmt->compile();
        return code; 
    }
    
    std::string gen3AC() override {
        std::string code = "";
        for (auto stmt : statements) {
            code += stmt->gen3AC();
        }
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "Block\n";
        for (auto stmt : statements) {
            res += stmt->getSyntaxTree(depth + 1);
        }
        return res;
    }
};

class IfNode : public ASTNode {
    ASTNode *cond, *body;
public:
    IfNode(ASTNode* c, ASTNode* b) : cond(c), body(b) {}
    std::string compile() override { 
        return "if (" + cond->compile() + ") {\n" + body->compile() + "}\n"; 
    }
    
    std::string gen3AC() override {
        std::string code = cond->gen3AC();
        std::string endLabel = newLabel();
        code += "ifFalse " + cond->getResult() + " goto " + endLabel + "\n";
        code += body->gen3AC();
        code += endLabel + ":\n";
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "If\n";
        res += getIndent(depth + 1) + "Condition:\n";
        res += cond->getSyntaxTree(depth + 2);
        res += getIndent(depth + 1) + "Then:\n";
        res += body->getSyntaxTree(depth + 2);
        return res;
    }
};

class WhileNode : public ASTNode {
    ASTNode *cond, *body;
public:
    WhileNode(ASTNode* c, ASTNode* b) : cond(c), body(b) {}
    std::string compile() override { 
        return "while (" + cond->compile() + ") {\n" + body->compile() + "}\n"; 
    }
    
    std::string gen3AC() override {
        std::string startLabel = newLabel();
        std::string endLabel = newLabel();
        
        std::string code = startLabel + ":\n";
        code += cond->gen3AC();
        code += "ifFalse " + cond->getResult() + " goto " + endLabel + "\n";
        code += body->gen3AC();
        code += "goto " + startLabel + "\n";
        code += endLabel + ":\n";
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "While\n";
        res += getIndent(depth + 1) + "Condition:\n";
        res += cond->getSyntaxTree(depth + 2);
        res += getIndent(depth + 1) + "Do:\n";
        res += body->getSyntaxTree(depth + 2);
        return res;
    }
};

class ForNode : public ASTNode {
    std::string iterVar;
    ASTNode *startExpr, *endExpr, *body;
public:
    ForNode(char* var, ASTNode* start, ASTNode* end, ASTNode* b) : iterVar(var), startExpr(start), endExpr(end), body(b) {}
    std::string compile() override { 
        std::string code = "setVar(\"" + iterVar + "\", " + startExpr->compile() + ");\n";
        code += "while (getVar(\"" + iterVar + "\").getVal() <= " + endExpr->compile() + ".getVal()) {\n";
        code += body->compile();
        code += "setVar(\"" + iterVar + "\", Value(getVar(\"" + iterVar + "\").getVal() + 1));\n";
        code += "}\n";
        return code; 
    }
    
    std::string gen3AC() override {
        std::string startLabel = newLabel();
        std::string endLabel = newLabel();
        std::string condTemp = newTemp();
        
        std::string code = startExpr->gen3AC() + endExpr->gen3AC();
        code += iterVar + " = " + startExpr->getResult() + "\n";
        
        code += startLabel + ":\n";
        code += condTemp + " = " + iterVar + " <= " + endExpr->getResult() + "\n";
        code += "ifFalse " + condTemp + " goto " + endLabel + "\n";
        
        code += body->gen3AC();
        code += iterVar + " = " + iterVar + " + 1\n";
        code += "goto " + startLabel + "\n";
        
        code += endLabel + ":\n";
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "For (" + iterVar + ")\n";
        res += getIndent(depth + 1) + "From:\n";
        res += startExpr->getSyntaxTree(depth + 2);
        res += getIndent(depth + 1) + "To:\n";
        res += endExpr->getSyntaxTree(depth + 2);
        res += getIndent(depth + 1) + "Do:\n";
        res += body->getSyntaxTree(depth + 2);
        return res;
    }
};

class BreakNode : public ASTNode {
public:
    std::string compile() override { return "break;\n"; }
    std::string gen3AC() override { return "goto [BREAK_LABEL_UNRESOLVED]\n"; } 
    std::string getSyntaxTree(int depth = 0) override { return getIndent(depth) + "Break\n"; }
};

class ContinueNode : public ASTNode {
public:
    std::string compile() override { return "continue;\n"; }
    std::string gen3AC() override { return "goto [CONTINUE_LABEL_UNRESOLVED]\n"; }
    std::string getSyntaxTree(int depth = 0) override { return getIndent(depth) + "Continue\n"; }
};

// --- Functions & Classes ---
class ClassDefNode : public ASTNode {
    std::string className;
public:
    ClassDefNode(char* name) : className(name) {}
    std::string compile() override { return ""; }
    std::string gen3AC() override { return "class " + className + "\n"; }
    std::string getSyntaxTree(int depth = 0) override { return getIndent(depth) + "ClassDef (" + className + ")\n"; }
};

class FunctionDefNode : public ASTNode {
    std::string funcName;
    std::vector<std::string>* args;
    ASTNode* body;
public:
    FunctionDefNode(char* name, std::vector<std::string>* a, ASTNode* b) : funcName(name), args(a), body(b) {}
    std::string compile() override { return ""; } 
    
    std::string gen3AC() override {
        std::string code = "func " + funcName + ":\n";
        for (const auto& arg : *args) {
            code += "pop_param " + arg + "\n";
        }
        code += body->gen3AC();
        code += "endfunc\n\n";
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "FunctionDef (" + funcName + ")\n";
        res += getIndent(depth + 1) + "Params:\n";
        for (const auto& arg : *args) {
            res += getIndent(depth + 2) + arg + "\n";
        }
        res += getIndent(depth + 1) + "Body:\n";
        res += body->getSyntaxTree(depth + 2);
        return res;
    }
};

class FunctionCallNode : public ASTNode {
    std::string funcName;
    std::vector<ASTNode*>* args;
    std::string resultTemp;
public:
    FunctionCallNode(char* name, std::vector<ASTNode*>* a) : funcName(name), args(a) { resultTemp = newTemp(); }
    std::string compile() override { return ""; } 
    
    std::string gen3AC() override {
        std::string code = "";
        for (auto arg : *args) {
            code += arg->gen3AC();
            code += "push_param " + arg->getResult() + "\n";
        }
        code += "call " + funcName + ", " + std::to_string(args->size()) + "\n";
        code += resultTemp + " = RET\n";
        return code;
    }
    std::string getResult() override { return resultTemp; }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "FunctionCall (" + funcName + ")\n";
        for (auto arg : *args) {
            res += arg->getSyntaxTree(depth + 1);
        }
        return res;
    }
};

// Add this class to your AST nodes section
class TypeOfNode : public ASTNode {
    ASTNode* expr;
    std::string resultTemp;
public:
    TypeOfNode(ASTNode* e) : expr(e) { resultTemp = newTemp(); }
    
    // Generates C++ code to return a string based on the Value's type field
    std::string compile() override { 
        std::string e = expr->compile();
        // Uses a ternary chain to determine the type string at runtime
        return "Value((" + e + ".type == 0) ? \"int\" : (" + e + ".type == 1) ? \"string\" : (" + 
               e + ".type == 2) ? \"object\" : (" + e + ".type == 3) ? \"float\" : \"array\")"; 
    }
    
    std::string gen3AC() override {
        std::string code = expr->gen3AC();
        code += resultTemp + " = typeof " + expr->getResult() + "\n";
        return code;
    }

    std::string getResult() override { return resultTemp; }

    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "TypeOf\n";
        res += expr->getSyntaxTree(depth + 1);
        return res;
    }
};


class ReturnNode : public ASTNode {
    ASTNode* expr;
public:
    ReturnNode(ASTNode* e) : expr(e) {}
    std::string compile() override { return ""; } 
    
    std::string gen3AC() override {
        std::string code = expr->gen3AC();
        code += "RET = " + expr->getResult() + "\n";
        code += "return\n";
        return code;
    }
    
    std::string getSyntaxTree(int depth = 0) override {
        std::string res = getIndent(depth) + "Return\n";
        res += expr->getSyntaxTree(depth + 1);
        return res;
    }
};

class SymbolTable {
public:
    void add(std::string name) {}
};

#endif