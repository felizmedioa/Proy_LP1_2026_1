#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const char* current_input;
static Token ungot_tokens[2];
static int ungot_count = 0;

void init_lexer(const char* input) {
    current_input = input;
    ungot_count = 0;
}

void unget_token(Token t) {
    if (ungot_count < 2) {
        ungot_tokens[ungot_count++] = t;
    }
}

Token get_next_token(void) {
    // Si tenemos tokens en el búfer, LIFO
    if (ungot_count > 0) {
        return ungot_tokens[--ungot_count];
    }

    Token token = {TOK_EOF, 0.0, ""};

    // Ignoramos los espacios en blanco
    while (*current_input != '\0' && isspace(*current_input)) {
        current_input++;
    }

    if (*current_input == '\0' || *current_input == '\n') {
        token.type = TOK_EOF;
        return token;
    }

    // Identificación de números
    if (isdigit(*current_input) || *current_input == '.') {
        char* endptr;
        token.type = TOK_NUMBER;
        token.value = strtod(current_input, &endptr);
        current_input = endptr;
        return token;
    }

    // Identificación de nombres de funciones o variables (letras y quizás números, no inicia con número)
    if (isalpha(*current_input) || *current_input == '_') {
        token.type = TOK_IDENTIFIER;
        int i = 0;
        while (*current_input != '\0' && (isalnum(*current_input) || *current_input == '_') && i < 31) {
            token.name[i++] = *current_input;
            current_input++;
        }
        token.name[i] = '\0';
        return token;
    }

    // Operadores y símbolos especiales
    switch (*current_input) {
        case '+': token.type = TOK_PLUS; break;
        case '-': token.type = TOK_MINUS; break;
        case '*': token.type = TOK_MUL; break;
        case '/': token.type = TOK_DIV; break;
        case '^': token.type = TOK_POW; break;
        case '%': token.type = TOK_MOD; break;
        case '(': token.type = TOK_LPAREN; break;
        case ')': token.type = TOK_RPAREN; break;
        case '=': token.type = TOK_ASSIGN; break;
        case ',': token.type = TOK_COMMA; break;
        default:
            token.type = TOK_ERROR;
            break;
    }

    current_input++; // Avanzar al siguiente carácter
    return token;
}
