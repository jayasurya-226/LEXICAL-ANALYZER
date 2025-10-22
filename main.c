#include <stdio.h>
#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include "colour.h"

const char* tokenTypeName(TokenType type){
    switch (type)
    {
        case KEYWORD: return "Keyword";
        case OPERATOR: return "Operator";
        case SPECIAL_CHARACTER: return "Special Character";
        case CONSTANT: return "Constant";
        case IDENTIFIER: return "Identifier";
        default: return "Unknown";
    }
}
const char* getColor(TokenType type) {
    switch (type) {
        case KEYWORD: return BLUE;
        case OPERATOR: return YELLOW;
        case SPECIAL_CHARACTER: return CYAN;
        case CONSTANT: return GREEN;
        case IDENTIFIER: return MAGENTA;
        default: return RED;
    }
}

int main(int argc, char *argv[]) {
   char filename[256], outFilename[256];
    FILE *outFile;

    if (argc < 2) {
        while (1) {
            printf("Enter the source file name (must be .c): ");
            if (scanf("%255s", filename) != 1) continue;
            if (!isCExtension(filename)) {
                printf(RED "This is not a .c file. Try again.\n" RESET);
                continue;
            }
            FILE *check = fopen(filename, "r");
            if (!check) {
                printf(RED "File not found. Try again.\n" RESET);
                continue;
            }
            fclose(check);
            break;
        }
    } else {
        strncpy(filename, argv[1], 255);
        filename[255] = '\0';
        if (!isCExtension(filename)) {
            printf(RED "This is not a .c file. Please run again with a .c file.\n" RESET);
            return EXIT_FAILURE;
        }
        FILE *check = fopen(filename, "r");
        if (!check) {
            printf(RED "File not found. Please run again with a valid file.\n" RESET);
            return EXIT_FAILURE;
        }
        fclose(check);
    }

    printf("Enter output file name (default: tokens.txt): ");
    scanf(" %255[^\n]", outFilename);
    if (strlen(outFilename) == 0) strcpy(outFilename, "tokens.txt");

    outFile = fopen(outFilename, "w");
    if (!outFile) {
        fprintf(stderr, RED "Error: Cannot open '%s' for writing\n" RESET, outFilename);
        return EXIT_FAILURE;
    }

    initializeLexer(filename);

    Token token;
    while (1) {
        token = getNextToken();
        if (strcmp(token.lexeme, "EOF") == 0) break;

        printf("%sToken: %-15s | Type: %s%s\n",
               getColor(token.type), token.lexeme,
               tokenTypeName(token.type), RESET);

        if (fprintf(outFile, "Token: %-15s | Type: %s\n", token.lexeme, tokenTypeName(token.type)) < 0) {
            fprintf(stderr, RED "Error writing to output file.\n" RESET);
            fclose(outFile);
            return EXIT_FAILURE;
        }
    }

    fclose(outFile);
    printf(GREEN "\nAll tokens saved to %s\n" RESET, outFilename);
    return EXIT_SUCCESS;
}
