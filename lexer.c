#include "lexer.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static const char* current_input;
static Token ungot_tokens[2];
static int ungot_count = 0;
static TokenType last_token_type = TOK_ERROR;

void init_lexer(const char* input) {
    current_input = input;
    ungot_count = 0;
    last_token_type = TOK_ERROR;
}

void unget_token(Token t) {
    if (ungot_count < 2) {
        ungot_tokens[ungot_count++] = t;
    }
}

Token get_next_token(void) {
    // Si tenemos tokens en el bufer, LIFO
    if (ungot_count > 0) {
        Token t = ungot_tokens[--ungot_count];
        last_token_type = t.type;
        return t;
    }

    Token token = {TOK_EOF, 0.0, ""};

    // Ignoramos los espacios en blanco
    while (*current_input != '\0' && isspace(*current_input)) {
        current_input++;
    }

    if (*current_input == '\0' || *current_input == '\n') {
        token.type = TOK_EOF;
        last_token_type = TOK_EOF;
        return token;
    }

    // Multiplicacion implicita
    if ((last_token_type == TOK_NUMBER || last_token_type == TOK_RPAREN) && 
        (isalpha(*current_input) || *current_input == '(')) {
        token.type = TOK_MUL;
        last_token_type = TOK_MUL;
        return token;
    }

    // Identificacion de numeros
    if (isdigit(*current_input) || *current_input == '.') {
        char* endptr;
        token.type = TOK_NUMBER;
        token.value = strtod(current_input, &endptr);
        current_input = endptr;
        last_token_type = TOK_NUMBER;
        return token;
    }

    // Identificacion de nombres de funciones o variables
    if (isalpha(*current_input) || *current_input == '_') {
        token.type = TOK_IDENTIFIER;
        int i = 0;
        while (*current_input != '\0' && (isalnum(*current_input) || *current_input == '_') && i < 31) {
            token.name[i++] = *current_input;
            current_input++;
        }
        token.name[i] = '\0';
        last_token_type = TOK_IDENTIFIER;
        return token;
    }

    // Operadores y simbolos especiales
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

    current_input++; // Avanzar al siguiente caracter
    last_token_type = token.type;
    return token;
}

