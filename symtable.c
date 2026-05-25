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
void symtable_set_const(const char* name, Complex value) {
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
    symtable_set_const("pi",  (Complex){M_PI, 0.0});
    symtable_set_const("e",   (Complex){M_E, 0.0});
    symtable_set_const("phi", (Complex){M_PHI, 0.0});
    symtable_set_const("i",   (Complex){0.0, 1.0});
}

// Retorna 1 si exitoso, 0 si fue bloqueado por ser constante
int symtable_set(const char* name, Complex value) {
    // Buscar si ya existe
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            // ¿Es una constante protegida?
            if (symbol_table[i].is_const) {
                printf("Error: '%s' es una constante y no puede modificarse.\n", name);
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

int symtable_get(const char* name, Complex* out_value) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            *out_value = symbol_table[i].value;
            return 1;
        }
    }
    return 0; // Variable no encontrada
}

// Función auxiliar para imprimir Complex sin nueva línea
static void print_complex_val(Complex c) {
    if (c.imag == 0.0) {
        printf("%g", c.real);
    } else if (c.real == 0.0) {
        if (c.imag == 1.0) printf("i");
        else if (c.imag == -1.0) printf("-i");
        else printf("%gi", c.imag);
    } else {
        if (c.imag > 0) {
            if (c.imag == 1.0) printf("%g + i", c.real);
            else printf("%g + %gi", c.real, c.imag);
        } else {
            if (c.imag == -1.0) printf("%g - i", c.real);
            else printf("%g - %gi", c.real, fabs(c.imag));
        }
    }
}

void symtable_list(void) {
    if (symbol_count == 0) {
        printf("No hay variables definidas.\n");
        return;
    }
    printf("Variables y constantes definidas:\n");
    for (int i = 0; i < symbol_count; i++) {
        printf("  %-10s = ", symbol_table[i].name);
        print_complex_val(symbol_table[i].value);
        if (symbol_table[i].is_const) {
            printf("  (constante)\n");
        } else {
            printf("\n");
        }
    }
}
