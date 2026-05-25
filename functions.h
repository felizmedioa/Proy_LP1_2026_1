#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "parser.h"

// Evalúa una función matemática dado su nombre y los argumentos
// 'args': arreglo con los valores evaluados.
// 'num_args': el número de argumentos de la función.
// 'out_result': referencia donde se guardará el resultado matemático.
// Retorna 1 si es exitosa, 0 si hay un error o la función no existe.
int call_function(const char* name, const Complex* args, int num_args, Complex* out_result);

// Configura la unidad angular (0 = Radianes, 1 = Grados)
void set_angle_unit(int use_degrees);
int get_angle_unit(void);

// Imprime la lista de todas las funciones matematicas disponibles
void list_functions(void);

// Rutinas estadisticas para datos agrupados (continua) y no agrupados (discreta)
void stat_calc_discrete(double* data, int n);
void stat_calc_continuous(double* lower_limits, double* upper_limits, double* freqs, int n);

#endif // FUNCTIONS_H
