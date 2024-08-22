<div>
  <img src="https://upload.wikimedia.org/wikipedia/commons/c/c3/Python-logo-notext.svg" alt="Python Logo" align="left" width="100" height="100">
  <h1>Python Interpreter</h1>
</div>

_A custom-built interpreter designed from the ground up to execute Python code. It also serves as an educational resource, promoting learning and encouraging contributions to the open-source Python ecosystem._

# Project Overview

Welcome to this Python Interpreter in C++ project! This project aims to provide a robust and feature-rich interpreter for the Python programming language, built entirely from scratch in C++. Whether you're looking to explore the internals of how an interpreter works, experiment with language features, or simply explore the intersection of C++ and Python, this interpreter is a powerful tool designed with flexibility and extensibility in mind. 

# 🔑 _Key Features_

  ### 🔧 _Support for Python's Built-in Data Types_
This interpreter already supports various built-in Python data types, enabling the manipulation of variables, strings, numbers, and more, similarly to the original Python.

  ### 🧠 _Dynamic Typing_
Just like Python, this interpreter supports dynamic typing, allowing variables to change types at runtime. This feature provides flexibility and mimics Python’s behavior, making it easier to write and execute dynamic code.

  ### 🗂️ _Classes and Instances_
Support for Object-Oriented Programming is implemented! You can define your own classes, create instances, and access attributes and methods just as you would in Python.

  ### ➕ _Operator Overloading_
The interpreter supports operator overloading, allowing you to define specific behaviors for operators like +, -, *, /, and more, within your custom classes.

### 📜 _Execution System and Parser_
The architecture of the interpreter includes a robust parser and execution system that translates Python code into executable operations. This allows for dynamic interpretation and real-time code execution.

 ### ♻️ _Garbage Collector_
To manage memory efficiently, the project includes an integrated garbage collector, which helps prevent memory leaks by automatically freeing unused objects.

## What This Interpreter Can Do
To illustrate the capabilities of this interpreter, consider the following Python code that demonstrates complex object-oriented features such as class definitions, method overloading, and recursive function calls. The interpreter is designed to handle these operations with precision, just as the standard Python interpreter would.

```python
class Number:
    def __init__(self, n):
        self._n = n
        
    def __add__(self, other):
        return Number(self._n + other._n)
    
    def __sub__(self, other):
        return Number(self._n - other._n)
    
    def __eq__(self, other):
        return self._n == other._n
    
    def __le__(self, other):
        return self._n <= other._n
    
    def get_value(self):
        return self._n

def fib(n):
    if n <= Number(1):
        return Number(1)
    else:
        return fib(n - Number(1)) + fib(n - Number(2))

def main():
    
    print(fib(Number(5)).get_value()) # 8

main()
```


# 🛠️ Building

```bash
$ git clone https://github.com/BRCode4Fun/Python-Interpreter
$ cd Python-Interpreter
$ make
```

# ✅ Running Tests

```bash
  $ make test
```

# 🚀 Future steps:

  + List, Set, Dict.
  + BigInt class with Karatsuba algorithm.
  + REPL mode.
  + Virtual Machine.
  + Better type checking and error handling.
  + Modules.
  + Iterator and generator objects.
  + Inheritance.
  + etc.

# 🤝 Contributions
This project is under continuous development, with new features being added regularly. If you’re passionate about Python, C++, or just interested in contributing to an exciting open-source project, feel free to jump in!

## How to Contribute

 + Fork this repository.
 + Create a new branch for your feature (git checkout -b feature/NewFeature).
 + Commit your changes (git commit -m 'Add NewFeature').
 + Push to the branch (git push origin feature/NewFeature).
 + Open a Pull Request.

Any form of contribution is welcome, whether it’s code, documentation, or testing. Additionally, feel free to share this project with friends, colleagues, or within developer communities.
