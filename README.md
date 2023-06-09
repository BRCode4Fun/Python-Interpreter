<div>
  <img src="https://upload.wikimedia.org/wikipedia/commons/c/c3/Python-logo-notext.svg" alt="Python Logo" align="left" width="100" height="100">
  <h1>Python Interpreter</h1>
</div>

Custom Python interpreter implemented in C++ to provide developers with a versatile and extensible tool for executing Python code, fostering learning, and encouraging contributions to the open-source Python ecosystem. 

## Project Overview

The Python Interpreter project aims to develop a fully functional interpreter for the Python programming language from scratch, using C++ as the implementation language. This project serves as an impressive demonstration of technical expertise and deep understanding of both Python and C++.

### Key Features

   + Language Parsing: The interpreter includes a robust parser capable of parsing Python source code and generating an abstract syntax tree (AST) representation.

   + Lexical Analysis: A comprehensive lexer/tokenizer module is implemented to break down the input source code into tokens, facilitating the subsequent parsing and interpretation stages.

   + Semantic Analysis: The interpreter performs semantic analysis on the parsed AST to detect and report any language-specific errors, such as undeclared variables or type mismatches.

   + Execution Engine: A powerful execution engine is designed to execute the parsed Python code, implementing the language's semantics and executing statements, expressions, and control flow constructs.

   + Standard Library Support: Efforts are made to provide support for a subset of Python's standard library modules, enabling the interpreter to execute a wide range of Python programs.

### Why This Project Matters

   * Showcasing Expertise: Developing a Python interpreter from scratch demonstrates a deep understanding of programming languages, language design, and compiler/interpreter construction. It highlights your ability to tackle complex projects and showcases your skills to potential employers or collaborators.

   * Technical Proficiency: By working on this project, you gain hands-on experience with language parsing, lexical analysis, semantic analysis, and execution engines. These skills are highly valuable for software development roles involving language design, compiler development, or performance optimization.

   * Curriculum Enhancement: Adding this project to your curriculum vitae (CV) or portfolio demonstrates your commitment to learning and improving your programming skills. It sets you apart from other candidates and shows your dedication to exploring advanced concepts in computer science.

## Building

```bash
$ git clone https://github.com/BRCode4Fun/Python-Interpreter
$ cd Python-Interpreter
$ make
```

## Running Tests

```bash
  $ make test
```
