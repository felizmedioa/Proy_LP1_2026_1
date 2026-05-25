#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "parser.h"

// Máxima cantidad de variables
#define MAX_VARIABLES 100

// Estructura de un símbolo (variable o constante)
typedef struct {
    char name[32];
    Complex value;
    int is_const; // 1 si es una constante protegida, 0 si es variable
} Symbol;

// Inicializa la tabla de símbolos e inserta constantes (pi, e, phi, i)
void symtable_init(void);

// Establece o actualiza el valor de una variable.
// Retorna 1 si fue exitoso, 0 si la variable es una constante protegida.
int symtable_set(const char* name, Complex value);

// Inserta un símbolo marcado como constante (solo para uso interno en init).
void symtable_set_const(const char* name, Complex value);

// Obtiene el valor de una variable. Retorna 1 si existe, 0 si no.
int symtable_get(const char* name, Complex* out_value);

// Muestra por pantalla todas las variables almacenadas (comando list)
void symtable_list(void);

#endif // SYMTABLE_H
