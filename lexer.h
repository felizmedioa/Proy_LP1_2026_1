#ifndef LEXER_H
#define LEXER_H

// Tipos de token que nuestro analizador léxico reconocerá
typedef enum {
    TOK_EOF,         // Fin de línea o de la entrada
    TOK_NUMBER,      // Un número (entero o punto flotante)
    TOK_IDENTIFIER,  // Nombre de una variable o de una función
    TOK_PLUS,        // '+'
    TOK_MINUS,       // '-'
    TOK_MUL,         // '*'
    TOK_DIV,         // '/'
    TOK_POW,         // '^'
    TOK_MOD,         // '%'
    TOK_LPAREN,      // '('
    TOK_RPAREN,      // ')'
    TOK_ASSIGN,      // '='
    TOK_COMMA,       // ',' para los argumentos de funciones max(a,b)
    TOK_ERROR        // Token inválido para manejo de errores
} TokenType;

// Estructura para almacenar información sobre el token actual
typedef struct {
    TokenType type;
    double value;           // Valor numérico si type == TOK_NUMBER
    char name[32];          // Cadena de texto si type == TOK_IDENTIFIER
} Token;

// Configura la cadena de caracteres a analizar
void init_lexer(const char* input);

// Obtiene el siguiente token de la entrada
Token get_next_token(void);

// Permite 'retroceder' un token (útil para el parser descendente)
void unget_token(Token t);

#endif // LEXER_H
