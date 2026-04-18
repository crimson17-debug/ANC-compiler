%{
#include "ast.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>

using namespace std;

void yyerror(const char *s);
int yylex(); 
extern FILE* yyin;

typedef struct yy_buffer_state *YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_create_buffer(FILE *file, int size);
extern void yypush_buffer_state(YY_BUFFER_STATE new_buffer);
extern void yypop_buffer_state();
extern int yylineno;

#ifndef YY_BUF_SIZE
#define YY_BUF_SIZE 16384
#endif

BlockNode* programRoot;
std::string functionDefinitions = ""; 
// Removed the duplicate map declaration

// NEW: Global map to track our Symbol Table during parsing
struct SymbolEntry {
    std::string type;
    int lineDeclared;
};
std::map<std::string, SymbolEntry> pseudoSymbolTable;
%}

%union {
    int num;
    double fval;
    char* str;
    class ASTNode* node;
    class BlockNode* block;
    std::vector<std::string>* str_list;
    std::vector<class ASTNode*>* node_list;
}
%expect 11
%token RTN IMPORT LBRACKET RBRACKET BRK CNT LPAREN RPAREN
%token FOR FROM ENDFOR FUNC ENDFUNC COMMA
%token <fval> FLOAT_LITERAL
%token <str> IDENTIFIER STRING
%token <num> NUMBER
%token TYPEOF
%token ASSIGN TO DISPLAY PLUS MINUS MULTIPLY DIVIDE INPUT
%token WHILE DO ENDWHILE IF THEN ENDIF LESS GREATER EQUALS
%token CLASS ENDCLASS NEW DOT

%type <node> statement expression condition
%type <block> statement_list program
%type <str_list> arg_list_def
%type <node_list> arg_list_call

%left PLUS MINUS
%left MULTIPLY DIVIDE
%left TYPEOF

%%

program:
    statement_list { programRoot = $1; }
    ;

statement_list:
    statement { $$ = new BlockNode(); $$->addStatement($1); }
    | statement_list statement { $1->addStatement($2); $$ = $1; }
    ;

statement:
    IDENTIFIER ASSIGN expression { 
        if(pseudoSymbolTable.find(string($1)) == pseudoSymbolTable.end()) {
            pseudoSymbolTable[string($1)] = {"Variable", yylineno};
        }
        $$ = new AssignmentNode($1, $3); free($1); 
    }
    | expression LBRACKET expression RBRACKET ASSIGN expression { 
        $$ = new ArrayAssignmentNode($1, $3, $6); 
    }
    | expression DOT IDENTIFIER ASSIGN expression { 
        pseudoSymbolTable[string($3)] = {"Object Property", yylineno};
        $$ = new PropertyAssignmentNode($1, $3, $5); free($3); 
    }
    | INPUT IDENTIFIER { 
        pseudoSymbolTable[string($2)] = {"I/O Variable", yylineno};
        $$ = new InputNode(new VariableNode($2)); 
    }
    | RTN expression { $$ = new ReturnNode($2); }
    | BRK { $$ = new BreakNode(); }
    | CNT { $$ = new ContinueNode(); }
    | DISPLAY expression { $$ = new PrintNode($2); }
    | WHILE condition DO statement_list ENDWHILE { $$ = new WhileNode($2, $4); }
    | IF condition THEN statement_list ENDIF { $$ = new IfNode($2, $4); }
    | FOR IDENTIFIER FROM expression TO expression DO statement_list ENDFOR { 
        pseudoSymbolTable[string($2)] = {"Loop Iterator", yylineno};
        $$ = new ForNode($2, $4, $6, $8); free($2); 
    }
    | CLASS IDENTIFIER ENDCLASS { 
        pseudoSymbolTable[string($2)] = {"Class Definition", yylineno};
        $$ = new ClassDefNode($2); free($2); 
    }
    | FUNC IDENTIFIER LPAREN arg_list_def RPAREN statement_list ENDFUNC { 
        pseudoSymbolTable[string($2)] = {"Function", yylineno};
        for (const auto& arg : *$4) {
            pseudoSymbolTable[arg] = {"Function Parameter", yylineno};
        }
        $$ = new FunctionDefNode($2, $4, $6); free($2); delete $4;
    }
    | IDENTIFIER LPAREN arg_list_call RPAREN { $$ = new FunctionCallNode($1, $3); free($1); delete $3; }
    | IMPORT STRING {
        FILE* f = fopen($2, "r");
        if (f) {
            yypush_buffer_state(yy_create_buffer(f, YY_BUF_SIZE));
            yyparse();
            yypop_buffer_state();
            fclose(f);
        }
        $$ = new BlockNode(); 
    }
    ;

arg_list_def:
    { $$ = new std::vector<std::string>(); }
    | IDENTIFIER { $$ = new std::vector<std::string>();
    $$->push_back(std::string($1)); free($1); }
    | arg_list_def COMMA IDENTIFIER { $1->push_back(std::string($3)); free($3); $$ = $1;
    }
    ;

arg_list_call:
    { $$ = new std::vector<ASTNode*>(); }
    | expression { $$ = new std::vector<ASTNode*>(); $$->push_back($1); }
    | arg_list_call COMMA expression { $1->push_back($3); $$ = $1; }
    ;

condition:
    expression LESS expression { $$ = new BinaryOpNode('<', $1, $3); }
    | expression GREATER expression { $$ = new BinaryOpNode('>', $1, $3); }
    | expression EQUALS expression { $$ = new BinaryOpNode('=', $1, $3); }
    ;

expression:
    NUMBER { $$ = new NumberNode($1); }
    | TYPEOF expression { $$ = new TypeOfNode($2); }
    | FLOAT_LITERAL { $$ = new FloatNode($1); }
    | STRING { $$ = new StringNode($1); free($1); }
    | IDENTIFIER { $$ = new VariableNode($1); free($1); }
    | expression LBRACKET expression RBRACKET { $$ = new ArrayAccessNode($1, $3); }
    | expression DOT IDENTIFIER { $$ = new PropertyAccessNode($1, $3); free($3); }
    | NEW IDENTIFIER { $$ = new NewNode($2); free($2); }
    | expression PLUS expression { $$ = new BinaryOpNode('+', $1, $3); }
    | expression MINUS expression { $$ = new BinaryOpNode('-', $1, $3); }
    | expression MULTIPLY expression { $$ = new BinaryOpNode('*', $1, $3); }
    | expression DIVIDE expression { $$ = new BinaryOpNode('/', $1, $3); }
    | LPAREN expression RPAREN { $$ = $2; }
    | IDENTIFIER LPAREN arg_list_call RPAREN { $$ = new FunctionCallNode($1, $3); free($1); delete $3; }
    ;

%%

void yyerror(const char *s) { 
    cerr << "Syntax Error at line " << yylineno << ": " << s << endl;
}

int main(int argc, char** argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            cerr << "Could not open file: " << argv[1] << endl;
            return 1;
        }
    }

    programRoot = nullptr;
    if (yyparse() != 0 || programRoot == nullptr) {
        cerr << "[Compiler] Build failed due to syntax errors. Aborting." << endl;
        return 1; 
    }

    // Check for "-o" flag to set custom executable name
    std::string exeName = "program"; // Default name
    if (argc >= 4 && std::string(argv[2]) == "-o") {
        exeName = argv[3];
    }

    // 1. Generate standard C++ Output
    string userCode = programRoot->compile();
    ofstream outFile("output.cpp");
    outFile << "#include <iostream>\n#include <string>\n#include <map>\n#include <vector>\n#include <cstdlib>\n\n";
    outFile << "struct Value {\n";
    outFile << "    int type;\n    int num;\n    double decimal;\n    std::string str;\n";
    outFile << "    std::map<std::string, Value>* properties;\n";
    outFile << "    std::map<int, Value>* arrayElements;\n";
    outFile << "    Value() : type(0), num(0), decimal(0.0), properties(nullptr), arrayElements(nullptr) {}\n";
    outFile << "    Value(int n) : type(0), num(n), decimal(0.0), properties(nullptr), arrayElements(nullptr) {}\n";
    outFile << "    Value(double d) : type(3), num(0), decimal(d), properties(nullptr), arrayElements(nullptr) {}\n";
    outFile << "    Value(std::string s) : type(1), num(0), decimal(0.0), str(s), properties(nullptr), arrayElements(nullptr) {}\n";
    outFile << "    Value(std::map<std::string, Value>* p) : type(2), num(0), decimal(0.0), properties(p), arrayElements(nullptr) {}\n";
    outFile << "    double getVal() {\n        if (type == 3) return decimal;\n        return (double)num;\n    }\n";
    outFile << "    Value& operator[](int index) {\n";
    outFile << "        if (arrayElements == nullptr) { type = 4; arrayElements = new std::map<int, Value>(); }\n";
    outFile << "        return (*arrayElements)[index];\n    }\n";
    outFile << "    void print() {\n";
    outFile << "        if (type == 2) std::cout << \"[Object Reference]\";\n";
    outFile << "        else if (type == 4) std::cout << \"[Array Reference]\";\n";
    outFile << "        else if (type == 1) std::cout << str;\n";
    outFile << "        else if (type == 3) std::cout << decimal;\n";
    outFile << "        else std::cout << num;\n    }\n";
    outFile << "    void readFromInput() {\n";
    outFile << "        std::string inputStr;\n";
    outFile << "        std::getline(std::cin >> std::ws, inputStr);\n";
    outFile << "        char* end;\n";
    outFile << "        long lval = strtol(inputStr.c_str(), &end, 10);\n";
    outFile << "        if (*end == '\\0') { type = 0; num = lval; return; }\n";
    outFile << "        double dval = strtod(inputStr.c_str(), &end);\n";
    outFile << "        if (*end == '\\0') { type = 3; decimal = dval; return; }\n";
    outFile << "        type = 1; str = inputStr;\n    }\n};\n\n";
    outFile << "std::vector<std::map<std::string, Value> > stack;\n";
    outFile << "std::map<std::string, Value> globals;\n\n";

    outFile << "Value& getVar(std::string name) {\n";
    outFile << "    if (!stack.empty() && stack.back().count(name)) return stack.back()[name];\n";
    outFile << "    return globals[name];\n}\n\n";
    outFile << "void setVar(std::string name, Value val) {\n";
    outFile << "    if (!stack.empty()) stack.back()[name] = val;\n";
    outFile << "    else globals[name] = val;\n}\n\n";
    outFile << "void push_scope() { stack.push_back(std::map<std::string, Value>()); }\n";
    outFile << "void pop_scope() { if (!stack.empty()) stack.pop_back(); }\n\n";
    outFile << functionDefinitions << "\n";
    outFile << "int main() {\n";
    outFile << userCode; 
    outFile << "    return 0;\n}\n";
    outFile.close();

    // 2. Generate Intermediate Code (C++ Translation)
    ofstream interFile("intermediate.txt");
    interFile << "--- INTERMEDIATE CODE (C++ TRANSLATION) ---\n\n";
    interFile << "// Function Definitions:\n" << functionDefinitions << "\n";
    interFile << "// Main Execution Block:\n" << userCode;
    interFile.close();

    // 3. Exhaustive Symbol Table Generation
    ofstream symFile("symbol_table.txt");
    symFile << "========================================================\n";
    symFile << "                 PSEUDO++ SYMBOL TABLE                  \n";
    symFile << "========================================================\n";
    symFile << "IDENTIFIER\t\tTYPE\t\t\tLINE DETECTED\n";
    symFile << "--------------------------------------------------------\n";
    for (auto const& [name, info] : pseudoSymbolTable) {
        symFile << name;
        if (name.length() < 8) symFile << "\t\t\t";
        else if (name.length() < 16) symFile << "\t\t";
        else symFile << "\t";
        
        symFile << info.type;
        if (info.type.length() < 12) symFile << "\t\t\t";
        else symFile << "\t\t";
        
        symFile << info.lineDeclared << "\n";
    }
    symFile.close();
    cout << "[Compiler] Generated symbol_table.txt" << endl;

    // 4. Full 3AC Generation
    ofstream tacFile("3ac.txt");
    tacFile << "--- THREE ADDRESS CODE (3AC) ---\n\n";
    tacFile << programRoot->gen3AC(); 
    tacFile.close();
    cout << "[Compiler] Generated 3ac.txt" << endl;

    // 5. Syntax Tree Generation
    ofstream treeFile("syntax_tree.txt");
    treeFile << "--- ABSTRACT SYNTAX TREE ---\n\n";
    treeFile << programRoot->getSyntaxTree(0);
    treeFile.close();
    cout << "[Compiler] Generated syntax_tree.txt" << endl;
    
    cout << "[Compiler] Building standalone executable..." << endl;
    std::string buildCommand = "g++ output.cpp -o " + exeName;
    int result = system(buildCommand.c_str());

    if (result == 0) {
        cout << "[Compiler] Success! Your code was compiled to '" << exeName << "'." << endl;
    } else {
        cout << "[Compiler] Error building executable." << endl;
    }

    return 0;
}