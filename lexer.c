#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

static FILE *sourceFile = NULL;

static const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct"
};

static const char* operators = "+-*/%=!<>|&";
static const char* multiCharOperators[] = {
    "==","<=",">=","!=","++","--","&&","||","+=","-=","*=","/=","%=","<<=",">>=","&=","|=", NULL};
static const char* specialCharacters = ",;{}()[]";

void initializeLexer(const char* filename) {
    sourceFile = fopen(filename, "r");
    if (!sourceFile) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        exit(EXIT_FAILURE);
    }
}

int isCExtension(const char *filename) {
    size_t len = strlen(filename);
    if (len < 2) return 0;
    const char *dot = strrchr(filename, '.');
    return (dot && strcmp(dot, ".c") == 0);
}

int isKeyword(const char* str) {
    for (int i = 0; i < MAX_KEYWORDS; i++) {
        if (!keywords[i]) break;
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

int isOperator(const char* str) {
    for (int i = 0; multiCharOperators[i]; i++)
        if (strcmp(str, multiCharOperators[i]) == 0) return 1;
    if (strlen(str) == 1 && strchr(operators, str[0]) != NULL) return 1;
    return 0;
}

int isSpecialCharacter(char ch) {
    return (strchr(specialCharacters, ch) != NULL);
}

int isConstant(const char* str) {
    size_t len = strlen(str);
    if (len == 0) return 0;
    if ((str[0] == '"' && str[len-1] == '"') || (str[0] == '\'' && str[len-1] == '\'')) return 1;

    int dot_count = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] == '.') dot_count++;
        else if (!isdigit((unsigned char)str[i])) return 0;
    }
    return dot_count <= 1;
}

int isIdentifier(const char* str) {
    if (!(isalpha((unsigned char)str[0]) || str[0] == '_')) return 0;
    for (int i = 1; str[i]; i++)
        if (!(isalnum((unsigned char)str[i]) || str[i] == '_')) return 0;
    return 1;
}

void categorizeToken(Token* token) {
    if (isKeyword(token->lexeme)) token->type = KEYWORD;
    else if (isOperator(token->lexeme)) token->type = OPERATOR;
    else if (isConstant(token->lexeme)) token->type = CONSTANT;
    else if (isIdentifier(token->lexeme)) token->type = IDENTIFIER;
    else token->type = UNKNOWN;
}

Token getNextToken() {
    Token token;
    token.lexeme[0] = '\0';
    token.type = UNKNOWN;

    int ch;

    // Skip whitespace and comments
    while ((ch = fgetc(sourceFile)) != EOF) {
        if (isspace(ch)) continue;

        // Handle comments: // single-line or /* multi-line */
        if (ch == '/') {
            int next = fgetc(sourceFile);
            if (next == '/') { 
                while ((ch = fgetc(sourceFile)) != EOF && ch != '\n'); // skip line comment
                continue;
            } else if (next == '*') {
                int prev = 0;
                while ((ch = fgetc(sourceFile)) != EOF) { // skip block comment
                    if (prev == '*' && ch == '/') break;
                    prev = ch;
                }
                continue;
            } else {
                ungetc(next, sourceFile); // not a comment, put char back
            }
        }
        break;
    }

    // End of file token
    if (ch == EOF) {
        strcpy(token.lexeme, "EOF");
        return token;
    }

    int i = 0;

    // Handle preprocessor directives like #include
    if (ch == '#') {
        token.lexeme[i++] = (char)ch;
        while ((ch = fgetc(sourceFile)) != EOF && !isspace(ch)) {
            if (i < MAX_TOKEN_SIZE - 1) token.lexeme[i++] = (char)ch;
        }
        token.lexeme[i] = '\0';
        token.type = PREPROCESSOR; // token type for preprocessor
        return token;
    }

    // Handle string literals
    if (ch == '"') {
        token.lexeme[i++] = '"';
        while ((ch = fgetc(sourceFile)) != EOF && ch != '"') {
            if (i < MAX_TOKEN_SIZE - 2) token.lexeme[i++] = (char)ch;
        }
        token.lexeme[i++] = '"';
        token.lexeme[i] = '\0';
        token.type = CONSTANT; // string literal
        return token;
    }

    // Handle character literals
    if (ch == '\'') {
        token.lexeme[i++] = '\'';
        while ((ch = fgetc(sourceFile)) != EOF && ch != '\'') {
            if (i < MAX_TOKEN_SIZE - 2) token.lexeme[i++] = (char)ch;
        }
        token.lexeme[i++] = '\'';
        token.lexeme[i] = '\0';
        token.type = CONSTANT; // char literal
        return token;
    }

    // Handle special characters
    if (isSpecialCharacter((char)ch)) {
        token.lexeme[i++] = (char)ch;
        token.lexeme[i] = '\0';
        token.type = SPECIAL_CHARACTER; // punctuation like ; , { } ( )
        return token;
    }

    // Handle operators (single or multi-character)
    if (strchr(operators, ch)) {
        token.lexeme[i++] = (char)ch;
        int next = fgetc(sourceFile);
        if (next != EOF) {
            char temp[3] = { (char)ch, (char)next, '\0' };
            for (int k = 0; multiCharOperators[k]; k++) {
                if (strcmp(temp, multiCharOperators[k]) == 0) {
                    token.lexeme[i++] = (char)next;
                    token.lexeme[i] = '\0';
                    token.type = OPERATOR; // multi-char operator like <=, ++
                    return token;
                }
            }
            ungetc(next, sourceFile); // not multi-char operator
        }
        token.lexeme[i] = '\0';
        token.type = OPERATOR; // single-char operator
        return token;
    }

    // Handle identifiers, numbers, and keywords
    while (ch != EOF && !isspace(ch) && !isSpecialCharacter((char)ch) && !strchr(operators, ch)) {
        if (i < MAX_TOKEN_SIZE - 1) token.lexeme[i++] = (char)ch;
        ch = fgetc(sourceFile);
    }
    if (ch != EOF) ungetc(ch, sourceFile); // put back last char if not part of token

    token.lexeme[i] = '\0';
    categorizeToken(&token); // determine if it's keyword, identifier, constant, etc.
    return token;
}
