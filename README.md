# Pseudo++ Compiler (`anc`)

Pseudo++ is a custom, readable programming language designed for algorithmic and object-oriented logic. 

This repository contains the complete compiler toolchain built from scratch using **Lex (Flex)** and **Yacc (Bison)**. It parses Pseudo++ source code, generates an Abstract Syntax Tree (AST), computes Three-Address Code (3AC), maintains a Symbol Table, and ultimately translates the code into a standalone, native C++ executable.

##  Language Features
* **Intuitive Syntax:** Easy-to-read keywords (`for i from 0 to 4 do`, `if x < y then`).
* **Data Types:** Supports integers, floating-point numbers, and strings.
* **Data Structures:** Built-in support for Arrays and Object properties.
* **Control Flow:** Nested loops (`while`, `for`), conditionals (`if`/`then`), and break/continue statements.
* **I/O Operations:** Simple input and output (`in`, `out`).

---

## ⚙️ Prerequisites

Because `anc` acts as a transpiler that converts Pseudo++ into C++ under the hood, **you must have a C++ compiler (`g++`) installed on your system** for it to generate the final executable.

* **Windows:** Install [MinGW-w64](https://www.mingw-w64.org/) and ensure the `bin` folder (containing `g++`) is added to your system's PATH.
* **macOS:** Open your terminal and run `xcode-select --install`
* **Linux:** Open your terminal and run `sudo apt install build-essential`

---

##  Installation (Pre-built Binaries)

You do not need to build the compiler from source. This repository uses a GitHub Actions CI/CD pipeline to automatically generate ready-to-use compilers for all major operating systems.

1. Go to the **[Actions](../../actions)** tab of this repository.
2. Click on the latest successful build (the green checkmark).
3. Scroll down to the **Artifacts** section at the bottom of the page.
4. Download the `.zip` file for your specific operating system (`windows-latest`, `macos-latest`, or `ubuntu-latest`).
5. Extract the `.zip` file to find your `anc` executable.

### ⚠️ Crucial Step: Add to PATH
To use the `anc` command from *any* folder on your computer, you must add the folder containing your extracted executable to your system's Environment Variables.
* **Windows:**
  1. Press the Windows key and search for "Environment Variables".
  2. Click "Edit the system environment variables".
  3. Click the `Environment Variables` button.
  4. Select the `Path` variable and click Edit.
  5. Click New and add the exact folder path where your `anc.exe` lives.
* **Mac/Linux:** Move the binary to your local bin directory by running: `sudo mv anc /usr/local/bin/`

---

##  Usage

Once installed and added to your PATH, you can use the compiler from your terminal exactly like `gcc`.

1. Create a file with your Pseudo++ code and save it with a `.anc` extension (e.g., `script.anc`).
2. Open your terminal in the same folder as your script.
3. Compile the script using the `-o` flag to specify the output name:
```bash
   anc script.anc -o my_app
```
   The compiler will generate your native executable alongside symbol_table.txt, 3ac.txt, and syntax_tree.txt for debugging. 
   
4.Run your generated program: 

5.Windows: .\my_app.exe 

5.Mac/Linux: ./my_app 

