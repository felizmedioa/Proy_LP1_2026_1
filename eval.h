#ifndef EVAL_H
#define EVAL_H

#include "parser.h"

// Variable global para indicar si ocurrió un error en tiempo de ejecución (ej. log(-1), div 0)
extern int eval_error;

// Evalúa recursivamente el Árbol de Sintaxis Abstracta (AST) y retorna el valor calculado.
double eval_ast(ASTNode* node);

#endif // EVAL_H
