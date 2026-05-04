#include "symtable.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.71828182845904523536
#endif

// Número áureo φ = (1 + sqrt(5)) / 2
#define M_PHI 1.61803398874989484820

static Symbol symbol_table[MAX_VARIABLES];
static int symbol_count = 0;

// Inserta una constante protegida (solo llamada desde symtable_init)
void symtable_set_const(const char* name, double value) {
    if (symbol_count < MAX_VARIABLES) {
        strcpy(symbol_table[symbol_count].name, name);
        symbol_table[symbol_count].value = value;
        symbol_table[symbol_count].is_const = 1;
        symbol_count++;
    }
}

void symtable_init(void) {
    symbol_count = 0;
    // Constantes predefinidas e inmutables
    symtable_set_const("pi",  M_PI);
    symtable_set_const("e",   M_E);
    symtable_set_const("phi", M_PHI);
}

// Retorna 1 si exitoso, 0 si fue bloqueado por ser constante
int symtable_set(const char* name, double value) {
    // Buscar si ya existe
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            // ¿Es una constante protegida?
            if (symbol_table[i].is_const) {
                printf("Error: '%s' es una constante y no puede modificarse "
                       "(valor actual: %g).\n", name, symbol_table[i].value);
                return 0;
            }
            symbol_table[i].value = value;
            return 1;
        }
    }

    // Si no existe, insertar como variable normal
    if (symbol_count < MAX_VARIABLES) {
        strcpy(symbol_table[symbol_count].name, name);
        symbol_table[symbol_count].value = value;
        symbol_table[symbol_count].is_const = 0;
        symbol_count++;
        return 1;
    }

    printf("Error: Tabla de símbolos llena.\n");
    return 0;
}

int symtable_get(const char* name, double* out_value) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            *out_value = symbol_table[i].value;
            return 1;
        }
    }
    return 0; // Variable no encontrada
}

void symtable_list(void) {
    if (symbol_count == 0) {
        printf("No hay variables definidas.\n");
        return;
    }
    printf("Variables y constantes definidas:\n");
    for (int i = 0; i < symbol_count; i++) {
        if (symbol_table[i].is_const) {
            printf("  %-10s = %-20g  (constante)\n",
                   symbol_table[i].name, symbol_table[i].value);
        } else {
            printf("  %-10s = %g\n",
                   symbol_table[i].name, symbol_table[i].value);
        }
    }
}
