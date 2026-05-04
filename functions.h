#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// Evalúa una función matemática dado su nombre y los argumentos
// 'args': arreglo con los valores evaluados.
// 'num_args': el número de argumentos de la función.
// 'out_result': referencia donde se guardará el resultado matemático.
// Retorna 1 si es exitosa, 0 si hay un error o la función no existe.
int call_function(const char* name, const double* args, int num_args, double* out_result);

// Configura la unidad angular (0 = Radianes, 1 = Grados)
void set_angle_unit(int use_degrees);
int get_angle_unit(void);

// Imprime la lista de todas las funciones matemáticas disponibles
void list_functions(void);

#endif // FUNCTIONS_H
