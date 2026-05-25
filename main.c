#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lexer.h"
#include "parser.h"
#include "eval.h"
#include "symtable.h"
#include "functions.h"

#define BUFFER_SIZE 1024

// Recorta los espacios y saltos de línea al final de un string.
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
    printf("Escribe una expresion (ej. a = 5 * sin(pi/2))\n");
    printf("Comandos especiales:\n");
    printf("  list   - Muestra las variables definidas\n");
    printf("  estad  - Calcula medidas de tendencia central y dispersion\n");
    printf("  funct  - Muestra las funciones disponibles\n");
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
        } else if (strcmp(input, "funct") == 0) {
            list_functions();
            continue;
        } else if (strcmp(input, "estad") == 0) {
            printf("Ingrese el tipo de variable estadistica (1 para Continua, 2 para Discreta): ");
            int tipo = 0;
            if (scanf("%d", &tipo) != 1) {
                printf("Entrada invalida.\n");
                while (getchar() != '\n'); // Limpiar buffer
                continue;
            }
            if (tipo == 2) {
                printf("Ingrese la cantidad total de datos (n): ");
                int n = 0;
                if (scanf("%d", &n) != 1 || n <= 0) {
                    printf("Cantidad invalida.\n");
                    while (getchar() != '\n');
                    continue;
                }
                double* data = (double*)malloc(n * sizeof(double));
                for (int i = 0; i < n; i++) {
                    printf("Dato %d: ", i + 1);
                    if (scanf("%lf", &data[i]) != 1) {
                        printf("Dato invalido, asumiendo 0.\n");
                        data[i] = 0.0;
                        while (getchar() != '\n');
                    }
                }
                stat_calc_discrete(data, n);
                free(data);
            } else if (tipo == 1) {
                printf("Desea ingresar datos libres (1) o datos ya tabulados (2)? ");
                int opcion_cont = 0;
                if (scanf("%d", &opcion_cont) != 1) {
                    printf("Opcion invalida.\n");
                    while (getchar() != '\n');
                    continue;
                }
                
                if (opcion_cont == 2) {
                    printf("Ingrese el numero de intervalos: ");
                    int n = 0;
                    if (scanf("%d", &n) != 1 || n <= 0) {
                        printf("Cantidad invalida.\n");
                        while (getchar() != '\n');
                        continue;
                    }
                    double* lim_inf = (double*)malloc(n * sizeof(double));
                    double* lim_sup = (double*)malloc(n * sizeof(double));
                    double* freqs = (double*)malloc(n * sizeof(double));
                    for (int i = 0; i < n; i++) {
                        printf("Intervalo %d - Limite inferior: ", i + 1);
                        if (scanf("%lf", &lim_inf[i]) != 1) { lim_inf[i] = 0; while (getchar() != '\n'); }
                        printf("Intervalo %d - Limite superior: ", i + 1);
                        if (scanf("%lf", &lim_sup[i]) != 1) { lim_sup[i] = 0; while (getchar() != '\n'); }
                        printf("Intervalo %d - Frecuencia absoluta: ", i + 1);
                        if (scanf("%lf", &freqs[i]) != 1) { freqs[i] = 0; while (getchar() != '\n'); }
                    }
                    stat_calc_continuous(lim_inf, lim_sup, freqs, n);
                    free(lim_inf);
                    free(lim_sup);
                    free(freqs);
                } else if (opcion_cont == 1) {
                    printf("Ingrese la cantidad total de datos libres (n): ");
                    int n = 0;
                    if (scanf("%d", &n) != 1 || n <= 0) {
                        printf("Cantidad invalida.\n");
                        while (getchar() != '\n');
                        continue;
                    }
                    double* data = (double*)malloc(n * sizeof(double));
                    double max_val = -1e99;
                    double min_val = 1e99;
                    for (int i = 0; i < n; i++) {
                        printf("Dato %d: ", i + 1);
                        if (scanf("%lf", &data[i]) != 1) {
                            printf("Dato invalido, asumiendo 0.\n");
                            data[i] = 0.0;
                            while (getchar() != '\n');
                        }
                        if (data[i] > max_val) max_val = data[i];
                        if (data[i] < min_val) min_val = data[i];
                    }
                    
                    if (n == 1) {
                        min_val = data[0];
                        max_val = data[0];
                    }
                    
                    double R = max_val - min_val;
                    int k = round(1 + 3.322 * log10(n));
                    if (k < 1) k = 1;
                    double A = R / k;
                    
                    double* lim_inf = (double*)malloc(k * sizeof(double));
                    double* lim_sup = (double*)malloc(k * sizeof(double));
                    double* freqs = (double*)calloc(k, sizeof(double));
                    
                    double current_min = min_val;
                    for (int i = 0; i < k; i++) {
                        lim_inf[i] = current_min;
                        lim_sup[i] = current_min + A;
                        current_min = lim_sup[i];
                    }
                    
                    for (int i = 0; i < n; i++) {
                        int index = -1;
                        for (int j = 0; j < k; j++) {
                            // Incluye el maximo en el ultimo intervalo
                            if (data[i] >= lim_inf[j] && (data[i] < lim_sup[j] || (j == k - 1 && data[i] <= lim_sup[j]))) {
                                index = j;
                                break;
                            }
                        }
                        if (index != -1) {
                            freqs[index]++;
                        }
                    }
                    
                    printf("\n(Agrupacion automatica con Regla de Sturges: Minimo=%g, Maximo=%g, Rango=%g, Intervalos=%d, Amplitud=%g)\n", min_val, max_val, R, k, A);
                    stat_calc_continuous(lim_inf, lim_sup, freqs, k);
                    
                    free(data);
                    free(lim_inf);
                    free(lim_sup);
                    free(freqs);
                } else {
                    printf("Opcion invalida.\n");
                }
            } else {
                printf("Tipo desconocido.\n");
            }
            while (getchar() != '\n'); // Limpiar el salto de linea restante
            continue;
        }

        // 1. Inicializamos Lexer
        init_lexer(input);

        // 2. Ejecutamos el Parser para obtener el AST
        ASTNode* ast = parse_expression();

        if (ast) { // Si el parsing fue exitoso
            // 3. Evaluamos la expresión
            eval_error = 0;
            Complex result = eval_ast(ast);
            
            if (!eval_error) {
                // Truncar valores muy pequeños debidos a la precisión del punto flotante
                if (fabs(result.real) < 1e-15) result.real = 0.0;
                if (fabs(result.imag) < 1e-15) result.imag = 0.0;

                if (result.imag == 0.0) {
                    printf("%g\n", result.real);
                } else if (result.real == 0.0) {
                    if (result.imag == 1.0) printf("i\n");
                    else if (result.imag == -1.0) printf("-i\n");
                    else printf("%gi\n", result.imag);
                } else {
                    if (result.imag > 0) {
                        if (result.imag == 1.0) printf("%g + i\n", result.real);
                        else printf("%g + %gi\n", result.real, result.imag);
                    } else {
                        if (result.imag == -1.0) printf("%g - i\n", result.real);
                        else printf("%g - %gi\n", result.real, fabs(result.imag));
                    }
                }
            }

            // 4. Liberamos memoria
            free_ast(ast);
        }
    }

    return 0;
}
