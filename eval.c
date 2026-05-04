#include "eval.h"
#include "symtable.h"
#include "functions.h"
#include <stdio.h>
#include <math.h>
//hola probando pullrequest
int eval_error = 0;

double eval_ast(ASTNode* node) {
    if (!node || eval_error) return 0.0;

    switch (node->type) {
        case AST_NUMBER:
            return node->value;
            
        case AST_VAR: {
            double val;
            if (!symtable_get(node->name, &val)) {
                printf("Error: Variable '%s' no definida.\n", node->name);
                eval_error = 1;
                return 0.0;
            }
            return val;
        }
            
        case AST_ASSIGN: {
            // El hijo derecho es la expresión a evaluar y guardar en el nombre
            double val = eval_ast(node->right);
            if (!eval_error) {
                // Si symtable_set retorna 0, la variable es una constante protegida.
                // El mensaje de error ya fue impreso; solo suprimimos la salida del resultado.
                if (!symtable_set(node->name, val)) {
                    eval_error = 1;
                }
            }
            return val;
        }
            
        case AST_FUNC: {
            // Evaluamos los argumentos de la función primero
            double args_eval[10]; // Asumimos un máximo de 10 argumentos por simplicidad
            if (node->num_args > 10) {
                printf("Error: Demasiados argumentos en función '%s'\n", node->name);
                eval_error = 1;
                return 0.0;
            }
            for (int i = 0; i < node->num_args; i++) {
                args_eval[i] = eval_ast(node->args[i]);
            }
            
            if (eval_error) return 0.0;
            
            double result;
            if (!call_function(node->name, args_eval, node->num_args, &result)) {
                eval_error = 1;
                return 0.0;
            }
            if (isnan(result)) { // Error matemático manejado en call_function
                eval_error = 1;
                return 0.0;
            }
            
            return result;
        }
            
        case AST_UNOP: {
            double val = eval_ast(node->left);
            if (node->op == TOK_MINUS) {
                return -val;
            }
            return val; // TOK_PLUS simplemente retorna el valor
        }
            
        case AST_BINOP: {
            double left_val = eval_ast(node->left);
            double right_val = eval_ast(node->right);
            
            if (eval_error) return 0.0;
            
            switch (node->op) {
                case TOK_PLUS: return left_val + right_val;
                case TOK_MINUS: return left_val - right_val;
                case TOK_MUL: return left_val * right_val;
                case TOK_DIV: {
                    if (right_val == 0.0) {
                        printf("Error matemático: División por cero.\n");
                        eval_error = 1;
                        return 0.0;
                    }
                    return left_val / right_val;
                }
                case TOK_POW: return pow(left_val, right_val);
                case TOK_MOD: {
                    if (right_val == 0.0) {
                        printf("Error matemático: Módulo por cero.\n");
                        eval_error = 1;
                        return 0.0;
                    }
                    return fmod(left_val, right_val);
                }
                default:
                    printf("Error interno: Operación binaria desconocida.\n");
                    eval_error = 1;
                    return 0.0;
            }
        }
        
        default:
            printf("Error interno: Nodo de AST desconocido.\n");
            eval_error = 1;
            return 0.0;
    }
}
