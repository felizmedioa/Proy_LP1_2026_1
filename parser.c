#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int parse_error = 0;

// Utilidades para manejo de memoria de nodos
static ASTNode* create_node(ASTNodeType type) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        printf("Error fatal: Sin memoria para AST.\n");
        exit(1);
    }
    node->type = type;
    node->left = NULL;
    node->right = NULL;
    node->args = NULL;
    node->num_args = 0;
    node->name[0] = '\0';
    return node;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    if (node->left) free_ast(node->left);
    if (node->right) free_ast(node->right);
    if (node->args) {
        for (int i = 0; i < node->num_args; i++) {
            free_ast(node->args[i]);
        }
        free(node->args);
    }
    free(node);
}

static void report_error(const char* msg) {
    if (!parse_error) {
        printf("Error de sintaxis: %s\n", msg);
        parse_error = 1;
    }
}

// Prototipos descendentes (Gramática recursiva)
static ASTNode* parse_assign(void);
static ASTNode* parse_add_sub(void);
static ASTNode* parse_mul_div_mod(void);
static ASTNode* parse_power(void);
static ASTNode* parse_unary(void);
static ASTNode* parse_primary(void);

// Expr -> Assign
ASTNode* parse_expression(void) {
    parse_error = 0;
    ASTNode* node = parse_assign();
    
    if (parse_error) {
        free_ast(node);
        return NULL;
    }
    
    // Verificamos que no haya basura después de la expresión analizada
    Token t = get_next_token();
    if (t.type != TOK_EOF && !parse_error) {
        report_error("Fueron encontrados símbolos inesperados al final.");
        free_ast(node);
        return NULL;
    }
    
    return node;
}

static ASTNode* parse_assign(void) {
    Token t1 = get_next_token();
    
    // Si tenemos Identificador = Expresión
    if (t1.type == TOK_IDENTIFIER) {
        Token t2 = get_next_token();
        if (t2.type == TOK_ASSIGN) {
            ASTNode* right = parse_assign(); // Soporte múltiple asignación
            if (!right) return NULL;
            
            ASTNode* node = create_node(AST_ASSIGN);
            strcpy(node->name, t1.name);
            node->right = right;
            return node;
        }
        // No era asignación, retrocedemos lo leído
        unget_token(t2);
    }
    unget_token(t1);
    
    return parse_add_sub();
}

static ASTNode* parse_add_sub(void) {
    ASTNode* node = parse_mul_div_mod();
    
    while (!parse_error) {
        Token t = get_next_token();
        if (t.type == TOK_PLUS || t.type == TOK_MINUS) {
            ASTNode* new_node = create_node(AST_BINOP);
            new_node->op = t.type;
            new_node->left = node;
            new_node->right = parse_mul_div_mod();
            node = new_node;
        } else {
            unget_token(t);
            break;
        }
    }
    return node;
}

static ASTNode* parse_mul_div_mod(void) {
    ASTNode* node = parse_power();
    
    while (!parse_error) {
        Token t = get_next_token();
        if (t.type == TOK_MUL || t.type == TOK_DIV || t.type == TOK_MOD) {
            ASTNode* new_node = create_node(AST_BINOP);
            new_node->op = t.type;
            new_node->left = node;
            new_node->right = parse_power();
            node = new_node;
        } else {
            unget_token(t);
            break;
        }
    }
    return node;
}

static ASTNode* parse_power(void) {
    ASTNode* node = parse_unary();
    
    Token t = get_next_token();
    if (t.type == TOK_POW) {
        ASTNode* new_node = create_node(AST_BINOP);
        new_node->op = t.type;
        new_node->left = node;
        new_node->right = parse_power(); // La potencia es asociativa por la derecha
        node = new_node;
    } else {
        unget_token(t);
    }
    return node;
}

static ASTNode* parse_unary(void) {
    Token t = get_next_token();
    if (t.type == TOK_PLUS || t.type == TOK_MINUS) {
        ASTNode* node = create_node(AST_UNOP);
        node->op = t.type;
        node->left = parse_unary();
        return node;
    }
    unget_token(t);
    return parse_primary();
}

static ASTNode* parse_primary(void) {
    Token t = get_next_token();
    
    if (t.type == TOK_NUMBER) {
        ASTNode* node = create_node(AST_NUMBER);
        node->value = t.value;
        return node;
    } 
    else if (t.type == TOK_LPAREN) {
        ASTNode* node = parse_assign(); // Resolvemos la expresión dentro del parentesis
        Token t2 = get_next_token();
        if (t2.type != TOK_RPAREN) {
            report_error("Falta un paréntesis de cierre ')'.");
        }
        return node;
    } 
    else if (t.type == TOK_IDENTIFIER) {
        char name[32];
        strcpy(name, t.name);
        
        Token t2 = get_next_token();
        if (t2.type == TOK_LPAREN) {
            // Es una llamada a función! (e.g. sin(x), max(a,b))
            ASTNode* node = create_node(AST_FUNC);
            strcpy(node->name, name);
            
            // Verificamos si la función está vacía f()
            Token t3 = get_next_token();
            if (t3.type == TOK_RPAREN) {
                // Función sin parámetros
                node->num_args = 0;
                return node;
            }
            unget_token(t3);
            
            // Argumentos (separados por comas)
            int capacity = 2;
            node->args = (ASTNode**)malloc(capacity * sizeof(ASTNode*));
            while (1) {
                if (node->num_args >= capacity) {
                    capacity *= 2;
                    node->args = (ASTNode**)realloc(node->args, capacity * sizeof(ASTNode*));
                }
                node->args[node->num_args++] = parse_assign();
                
                Token sep = get_next_token();
                if (sep.type == TOK_RPAREN) {
                    break;
                } else if (sep.type != TOK_COMMA) {
                    report_error("Se esperaba ',' o ')' en la llamada a la función.");
                    break;
                }
            }
            return node;
        } else {
            // Era únicamente una variable
            unget_token(t2);
            ASTNode* node = create_node(AST_VAR);
            strcpy(node->name, name);
            return node;
        }
    }
    
    report_error("Expresión o símbolo inválido esperado (número, variable o función).");
    return NULL;
}
