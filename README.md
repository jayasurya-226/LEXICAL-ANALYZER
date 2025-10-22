# LEXICAL-ANALYZER
Lexical Analyzer in C: This project implements a lexer that reads C source code and converts it into tokens such as keywords, identifiers, operators, constants, and delimiters. It ignores comments/whitespace, detects invalid tokens, and outputs a list of tokens with type and value.

## Project Overview
This project implements a **Lexical Analyzer (Lexer)** in **C**. It reads C source code and converts it into **tokens**, the smallest meaningful units in programming, such as **keywords, identifiers, constants, operators, and delimiters**.  
The lexer ignores comments and whitespace, detects invalid tokens, and outputs a list of tokens with their type and value. This is the **first phase of a compiler**, demonstrating fundamental compiler design concepts.

## Features
- Recognizes **keywords, identifiers, constants, operators, and delimiters**.  
- Ignores **single-line (`//`) and multi-line (`/* */`) comments**.  
- Detects and reports **invalid tokens**.  
- Outputs **tokens with their types** in an organized format.  
- Modular and easy to **extend for additional languages or token rules**.

## Technologies Used
- **Language:** C  
- **Compiler:** GCC  
- **Concepts:** File handling, string processing, tokenization, symbol table management, error handling  

