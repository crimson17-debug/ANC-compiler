@echo off
echo --- BUILDING COMPILER TOOLCHAIN ---
.\win_bison.exe -d parser.y
.\win_flex.exe lexer.l     

REM Compile the compiler and name it "anc.exe"
g++ parser.tab.c lex.yy.c -o anc.exe

echo.
echo --- COMPILER BUILT SUCCESSFULLY ---
echo anc.exe is ready to use!