#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include "symtable.h"
#include "functions.h"

#define BUFFER_SIZE 1024

// Recorta los espacios y saltos de línea al final de un string
void trim_newline(char* str) {
    int len = strlen(str);
    while (len > 0 && (str[len-1] == '\n' || str[len-1] == '\r' || str[len-1] == ' ')) {
        str[len-1] = '\0';
        len--;
    }
}

int main(void) {
    char input[BUFFER_SIZE];
    
    printf("====================================================\n");
    printf("   Mini Evaluador de Expresiones Algebraicas (P11)  \n");
    printf("====================================================\n");
    printf("Escribe una expresión (ej. a = 5 * sin(pi/2))\n");
    printf("Comandos especiales:\n");
    printf("  list   - Muestra las variables definidas\n");
    printf("  quit   - Sale del programa\n");
    printf("  deg    - Cambia el modo a Grados\n");
    printf("  rad    - Cambia el modo a Radianes (por defecto)\n");
    printf("====================================================\n");

    // Inicializamos la tabla de símbolos (cargará 'pi' y 'e')
    symtable_init();

    while (1) {
        printf(">> ");
        if (!fgets(input, BUFFER_SIZE, stdin)) {
            break; // EOF
        }

        trim_newline(input);

        // Si el usuario simplemente presionó enter sin texto
        if (strlen(input) == 0) {
            continue;
        }

        // Procesamiento de comandos rápidos
        if (strcmp(input, "quit") == 0 || strcmp(input, "exit") == 0) {
            printf("Saliendo del evaluador...\n");
            break;
        } else if (strcmp(input, "list") == 0) {
            symtable_list();
            continue;
        } else if (strcmp(input, "deg") == 0) {
            set_angle_unit(1);
            printf("Modo angular cambiado a GRADOS.\n");
            continue;
        } else if (strcmp(input, "rad") == 0) {
            set_angle_unit(0);
            printf("Modo angular cambiado a RADIANES.\n");
            continue;
        }

        // 1. Inicializamos Lexer
        init_lexer(input);

        // 2. Ejecutamos el Parser para obtener el AST
        ASTNode* ast = parse_expression();

        if (ast) { // Si el parsing fue exitoso
            // 3. Evaluamos la expresión
            eval_error = 0;
            double result = eval_ast(ast);
            
            if (!eval_error) {
                // GeoGebra Style format:
                printf("%g\n", result);
            }

            // 4. Liberamos memoria
            free_ast(ast);
        }
    }

    return 0;
}
