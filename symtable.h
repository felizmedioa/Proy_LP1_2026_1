#ifndef SYMTABLE_H
#define SYMTABLE_H

// Máxima cantidad de variables
#define MAX_VARIABLES 100

// Estructura de un símbolo (variable)
typedef struct {
    char name[32];
    double value;
} Symbol;

// Inicializa la tabla de símbolos e inserta constantes (e.g., pi, e)
void symtable_init(void);

// Establece o actualiza el valor de una variable
void symtable_set(const char* name, double value);

// Obtiene el valor de una variable. Retorna 1 si existe, 0 si no.
int symtable_get(const char* name, double* out_value);

// Muestra por pantalla todas las variables almacenadas (comando list)
void symtable_list(void);

#endif // SYMTABLE_H
