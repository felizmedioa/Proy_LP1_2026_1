#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"//sadasdasd

// Tipos de nodos en nuestro Árbol de Sintaxis Abstracta (AST)
typedef enum {
    AST_NUMBER,    // 5.5
    AST_VAR,       // pi, my_var
    AST_BINOP,     // a + b
    AST_UNOP,      // -a
    AST_FUNC,      // sin(x), max(a, b)
    AST_ASSIGN     // a = b
} ASTNodeType;

// Estructura para números complejos
typedef struct {
    double real;
    double imag;
} Complex;

// Declaración adelantada de la estructura Node
struct ASTNode;

// Nodo del AST
typedef struct ASTNode {
    ASTNodeType type;
    
    // Valor directo (AST_NUMBER) o tipo de operador (+, -, etc para AST_BINOP / AST_UNOP)
    union {
        Complex value;
        TokenType op;      
    };
    
    // Nombre de variable (AST_VAR, AST_ASSIGN) o función (AST_FUNC)
    char name[32];
    
    // Hijos en el árbol (AST_BINOP usa left y right; AST_UNOP usa left)
    struct ASTNode* left;
    struct ASTNode* right;

    // Para funciones con múltiples argumentos (como atan2 o max)
    struct ASTNode** args; 
    int num_args;
} ASTNode;

// Inicia el parsing y retorna la raíz del AST generado (nulo si hay error sintáctico)
ASTNode* parse_expression(void);

// Libera recursivamente la memoria ocupada por el AST
void free_ast(ASTNode* node);

// Variables externas para reportar errores
extern int parse_error;

#endif // PARSER_H
