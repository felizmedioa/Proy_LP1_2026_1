#include "eval.h"
#include "symtable.h"
#include "functions.h"
#include <stdio.h>
#include <math.h>

int eval_error = 0;

Complex eval_ast(ASTNode* node) {
    if (!node || eval_error) return (Complex){0.0, 0.0};

    switch (node->type) {
        case AST_NUMBER:
            return node->value;
            
        case AST_VAR: {
            Complex val;
            if (!symtable_get(node->name, &val)) {
                printf("Error: Variable '%s' no definida.\n", node->name);
                eval_error = 1;
                return (Complex){0.0, 0.0};
            }
            return val;
        }
            
        case AST_ASSIGN: {
            Complex val = eval_ast(node->right);
            if (!eval_error) {
                if (!symtable_set(node->name, val)) {
                    eval_error = 1;
                }
            }
            return val;
        }
            
        case AST_FUNC: {
            Complex args_eval[10];
            if (node->num_args > 10) {
                printf("Error: Demasiados argumentos en función '%s'\n", node->name);
                eval_error = 1;
                return (Complex){0.0, 0.0};
            }
            for (int i = 0; i < node->num_args; i++) {
                args_eval[i] = eval_ast(node->args[i]);
            }
            
            if (eval_error) return (Complex){0.0, 0.0};
            
            Complex result;
            if (!call_function(node->name, args_eval, node->num_args, &result)) {
                eval_error = 1;
                return (Complex){0.0, 0.0};
            }
            if (isnan(result.real) || isnan(result.imag)) {
                eval_error = 1;
                return (Complex){0.0, 0.0};
            }
            
            return result;
        }
            
        case AST_UNOP: {
            Complex val = eval_ast(node->left);
            if (node->op == TOK_MINUS) {
                return (Complex){-val.real, val.imag == 0.0 ? 0.0 : -val.imag};
            }
            return val;
        }
            
        case AST_BINOP: {
            Complex left_val = eval_ast(node->left);
            Complex right_val = eval_ast(node->right);
            
            if (eval_error) return (Complex){0.0, 0.0};
            
            switch (node->op) {
                case TOK_PLUS: 
                    return (Complex){left_val.real + right_val.real, left_val.imag + right_val.imag};
                case TOK_MINUS: 
                    return (Complex){left_val.real - right_val.real, left_val.imag - right_val.imag};
                case TOK_MUL: 
                    return (Complex){
                        left_val.real * right_val.real - left_val.imag * right_val.imag,
                        left_val.real * right_val.imag + left_val.imag * right_val.real
                    };
                case TOK_DIV: {
                    double denom = right_val.real * right_val.real + right_val.imag * right_val.imag;
                    if (denom == 0.0) {
                        printf("Error matemático: División por cero.\n");
                        eval_error = 1;
                        return (Complex){0.0, 0.0};
                    }
                    return (Complex){
                        (left_val.real * right_val.real + left_val.imag * right_val.imag) / denom,
                        (left_val.imag * right_val.real - left_val.real * right_val.imag) / denom
                    };
                }
                case TOK_POW: {
                    double r = sqrt(left_val.real * left_val.real + left_val.imag * left_val.imag);
                    if (r == 0.0) {
                        if (right_val.real == 0.0 && right_val.imag == 0.0) return (Complex){1.0, 0.0};
                        if (right_val.real < 0.0) {
                            printf("Error matemático: División por cero en potencia.\n");
                            eval_error = 1;
                            return (Complex){0.0, 0.0};
                        }
                        return (Complex){0.0, 0.0};
                    }
                    double theta = atan2(left_val.imag, left_val.real);
                    double ln_r = log(r);
                    
                    double real_part = right_val.real * ln_r - right_val.imag * theta;
                    double imag_part = right_val.real * theta + right_val.imag * ln_r;
                    
                    double e_real = exp(real_part);
                    return (Complex){e_real * cos(imag_part), e_real * sin(imag_part)};
                }
                case TOK_MOD: {
                    if (right_val.real == 0.0 && right_val.imag == 0.0) {
                        printf("Error matemático: Módulo por cero.\n");
                        eval_error = 1;
                        return (Complex){0.0, 0.0};
                    }
                    if (left_val.imag == 0.0 && right_val.imag == 0.0) {
                        return (Complex){fmod(left_val.real, right_val.real), 0.0};
                    } else {
                        printf("Error matemático: Módulo con números complejos no soportado.\n");
                        eval_error = 1;
                        return (Complex){0.0, 0.0};
                    }
                }
                default:
                    printf("Error interno: Operación binaria desconocida.\n");
                    eval_error = 1;
                    return (Complex){0.0, 0.0};
            }
        }
        
        default:
            printf("Error interno: Nodo de AST desconocido.\n");
            eval_error = 1;
            return (Complex){0.0, 0.0};
    }
}
