#include "functions.h"
#include <string.h>
#include <math.h>
#include <stdio.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 0 = Radianes (por defecto), 1 = Grados
static int angle_mode = 0;

void set_angle_unit(int use_degrees) {
    angle_mode = use_degrees;
}

int get_angle_unit(void) {
    return angle_mode;
}

// Factor de conversión para funciones trigonométricas
static double apply_angle_factor(double val) {
    if (angle_mode == 1) { // Grados a radianes
        return val * (M_PI / 180.0);
    }
    return val;
}

// Radianes a grados
static double remove_angle_factor(double val) {
    if (angle_mode == 1) {
        return val * (180.0 / M_PI);
    }
    return val;
}

// Prototipo interno del puntero a función
typedef double (*MathFunc1)(double);
typedef double (*MathFunc2)(double, double);

// Estructura de la tabla de funciones
typedef struct {
    char name[16];
    int num_args;
    MathFunc1 func1;
    MathFunc2 func2;
} FuncEntry;

// Wrappers para funciones de rango u otras particulares
double f_max(double a, double b) { return (a > b) ? a : b; }
double f_min(double a, double b) { return (a < b) ? a : b; }

// Wrappers Trigonométricos con soporte para grados y validaciones
double f_sin(double x) { return sin(apply_angle_factor(x)); }
double f_cos(double x) { return cos(apply_angle_factor(x)); }
double f_tan(double x) { return tan(apply_angle_factor(x)); }

double f_asin(double x) { 
    if (x < -1.0 || x > 1.0) {
        printf("Error de Dominio: asin(%g) fuera del rango [-1, 1].\n", x);
        return NAN;
    }
    return remove_angle_factor(asin(x)); 
}
double f_acos(double x) { 
    if (x < -1.0 || x > 1.0) {
        printf("Error de Dominio: acos(%g) fuera del rango [-1, 1].\n", x);
        return NAN;
    }
    return remove_angle_factor(acos(x)); 
}
double f_atan(double x) { return remove_angle_factor(atan(x)); }
double f_atan2(double y, double x) { return remove_angle_factor(atan2(y, x)); }

double f_log(double x) {
    if (x <= 0.0) {
        printf("Error de Dominio: log(%g) negativo o cero.\n", x);
        return NAN;
    }
    return log(x);
}
double f_log2(double x) {
    if (x <= 0.0) {
        printf("Error de Dominio: log2(%g) negativo o cero.\n", x);
        return NAN;
    }
    return log2(x);
}
double f_log10(double x) {
    if (x <= 0.0) {
        printf("Error de Dominio: log10(%g) negativo o cero.\n", x);
        return NAN;
    }
    return log10(x);
}
double f_sqrt(double x) {
    if (x < 0.0) {
        printf("Error de Dominio: sqrt(%g) negativo.\n", x);
        return NAN;
    }
    return sqrt(x);
}


// Tabla de callbacks para funciones
static FuncEntry func_table[] = {
    {"sin", 1, f_sin, NULL},
    {"cos", 1, f_cos, NULL},
    {"tan", 1, f_tan, NULL},
    {"asin", 1, f_asin, NULL},
    {"acos", 1, f_acos, NULL},
    {"atan", 1, f_atan, NULL},
    {"atan2", 2, NULL, f_atan2},
    {"log", 1, f_log, NULL},
    {"log2", 1, f_log2, NULL},
    {"log10", 1, f_log10, NULL},
    {"exp", 1, exp, NULL},
    {"sqrt", 1, f_sqrt, NULL},
    {"abs", 1, fabs, NULL},
    {"max", 2, NULL, f_max},
    {"min", 2, NULL, f_min},
    {"pow", 2, NULL, pow}
};
static int num_functions = sizeof(func_table) / sizeof(func_table[0]);


int call_function(const char* name, const double* args, int num_args, double* out_result) {
    for (int i = 0; i < num_functions; i++) {
        if (strcmp(func_table[i].name, name) == 0) {
            
            if (func_table[i].num_args != num_args) {
                printf("Error: '%s' espera %d argumento(s), pero se dieron %d.\n", name, func_table[i].num_args, num_args);
                return 0; // Error de invocacion
            }

            if (num_args == 1) {
                *out_result = func_table[i].func1(args[0]);
            } else if (num_args == 2) {
                *out_result = func_table[i].func2(args[0], args[1]);
            }
            return 1; // Exitoso
        }
    }
    
    printf("Error: Función desconocida '%s'\n", name);
    return 0;
}
