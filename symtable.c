#include "symtable.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

static Symbol symbol_table[MAX_VARIABLES];
static int symbol_count = 0;

void symtable_init(void) {
    symbol_count = 0;
    // Constantes predefinidas
    symtable_set("pi", 3.14159265358979323846);
    symtable_set("e",  2.71828182845904523536);
}

void symtable_set(const char* name, double value) {
    // Buscar si ya existe la actualizamos
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            symbol_table[i].value = value;
            return;
        }
    }
    
    // Si no existe, agregamos a la tabla si hay espacio
    if (symbol_count < MAX_VARIABLES) {
        strcpy(symbol_table[symbol_count].name, name);
        symbol_table[symbol_count].value = value;
        symbol_count++;
    } else {
        printf("Error: Tabla de símbolos llena.\n");
    }
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
    printf("Variables definidas:\n");
    for (int i = 0; i < symbol_count; i++) {
        printf("  %s = %g\n", symbol_table[i].name, symbol_table[i].value);
    }
}
