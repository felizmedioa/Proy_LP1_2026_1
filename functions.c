#include "functions.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 0 = Radianes (por defecto), 1 = Grados
static int angle_mode = 0;

void set_angle_unit(int use_degrees) { angle_mode = use_degrees; }
int get_angle_unit(void) { return angle_mode; }

// Factor de conversion
static double apply_angle_factor(double val) {
    if (angle_mode == 1) return val * (M_PI / 180.0);
    return val;
}
static double remove_angle_factor(double val) {
    if (angle_mode == 1) return val * (180.0 / M_PI);
    return val;
}

// Prototipos internos
typedef double (*MathFunc1)(double);
typedef double (*MathFunc2)(double, double);
typedef double (*MathFuncN)(const Complex*, int);
typedef Complex (*MathFuncNComplex)(const Complex*, int);

typedef struct {
    char name[16];
    int num_args;
    MathFunc1 func1;
    MathFunc2 func2;
    MathFuncN funcN;
    MathFuncNComplex funcNC;
} FuncEntry;

// Wrappers basicos
double f_max(double a, double b) { return (a > b) ? a : b; }
double f_min(double a, double b) { return (a < b) ? a : b; }

double f_sin(double x) { return sin(apply_angle_factor(x)); }
double f_cos(double x) { return cos(apply_angle_factor(x)); }
double f_tan(double x) { return tan(apply_angle_factor(x)); }
double f_asin(double x) { 
    if (x < -1.0 || x > 1.0) { printf("Error: asin(%g) fuera de [-1, 1].\n", x); return NAN; }
    return remove_angle_factor(asin(x)); 
}
double f_acos(double x) { 
    if (x < -1.0 || x > 1.0) { printf("Error: acos(%g) fuera de [-1, 1].\n", x); return NAN; }
    return remove_angle_factor(acos(x)); 
}
double f_atan(double x) { return remove_angle_factor(atan(x)); }
double f_atan2(double y, double x) { return remove_angle_factor(atan2(y, x)); }
double f_ln(double x) {
    if (x <= 0.0) { printf("Error: ln(%g) debe ser > 0.\n", x); return NAN; }
    return log(x);
}
double f_log_base(double a, double b) {
    if (a <= 0.0 || a == 1.0 || b <= 0.0) { printf("Error: log(a,b) argumentos invalidos.\n"); return NAN; }
    return log(b) / log(a);
}
double f_log2(double x) {
    if (x <= 0.0) { printf("Error: log2(%g) <= 0.\n", x); return NAN; }
    return log2(x);
}
double f_log10(double x) {
    if (x <= 0.0) { printf("Error: log10(%g) <= 0.\n", x); return NAN; }
    return log10(x);
}
double f_sqrt(double x) {
    if (x < 0.0) { printf("Error: sqrt(%g) < 0.\n", x); return NAN; }
    return sqrt(x);
}

// --- Operaciones con Numeros Complejos (Helper) ---
static Complex c_add(Complex a, Complex b) { return (Complex){a.real + b.real, a.imag + b.imag}; }
static Complex c_sub(Complex a, Complex b) { return (Complex){a.real - b.real, a.imag - b.imag}; }
static Complex c_mul(Complex a, Complex b) { 
    return (Complex){a.real * b.real - a.imag * b.imag, a.real * b.imag + a.imag * b.real}; 
}
static Complex c_div(Complex a, Complex b) {
    double denom = b.real * b.real + b.imag * b.imag;
    if (denom == 0) return (Complex){0, 0};
    return (Complex){(a.real * b.real + a.imag * b.imag) / denom, (a.imag * b.real - a.real * b.imag) / denom};
}
static Complex c_sqrt(Complex a) {
    double r = sqrt(a.real * a.real + a.imag * a.imag);
    double theta = atan2(a.imag, a.real);
    double root_r = sqrt(r);
    return (Complex){root_r * cos(theta / 2.0), root_r * sin(theta / 2.0)};
}
static void c_cbrt_all(Complex a, Complex out[3]) {
    double r = sqrt(a.real * a.real + a.imag * a.imag);
    double theta = atan2(a.imag, a.real);
    double cr = pow(r, 1.0 / 3.0);
    for (int k = 0; k < 3; k++) {
        double angle = (theta + 2.0 * M_PI * k) / 3.0;
        out[k] = (Complex){cr * cos(angle), cr * sin(angle)};
    }
}

// --- Resolucion de Ecuaciones ---
static void print_roots(const char* name, Complex* roots, int n) {
    printf("Raices halladas en %s:\n", name);
    for (int i = 0; i < n; i++) {
        Complex r = roots[i];
        if (fabs(r.real) < 1e-15) r.real = 0.0;
        if (fabs(r.imag) < 1e-15) r.imag = 0.0;
        printf("  x%d = ", i + 1);
        if (r.imag == 0.0) {
            printf("%g\n", r.real);
        } else if (r.real == 0.0) {
            if (r.imag == 1.0) printf("i\n");
            else if (r.imag == -1.0) printf("-i\n");
            else printf("%gi\n", r.imag);
        } else {
            if (r.imag > 0) {
                if (r.imag == 1.0) printf("%g + i\n", r.real);
                else printf("%g + %gi\n", r.real, r.imag);
            } else {
                if (r.imag == -1.0) printf("%g - i\n", r.real);
                else printf("%g - %gi\n", r.real, fabs(r.imag));
            }
        }
    }
}

static Complex solve1(const Complex* args, int n) {
    if (n != 2) return (Complex){0, 0};
    Complex a = args[0], b = args[1];
    if (a.real == 0 && a.imag == 0) { printf("Error: coeficiente principal es 0.\n"); return (Complex){0,0}; }
    Complex roots[1] = { c_div(c_sub((Complex){0,0}, b), a) };
    print_roots("solve1", roots, 1);
    return roots[0];
}

static void solve2_internal(const Complex* args, Complex* roots) {
    Complex a = args[0], b = args[1], c = args[2];
    Complex b2 = c_mul(b, b);
    Complex ac4 = c_mul((Complex){4,0}, c_mul(a, c));
    Complex disc = c_sub(b2, ac4);
    Complex sq = c_sqrt(disc);
    Complex neg_b = c_sub((Complex){0,0}, b);
    Complex a2 = c_mul((Complex){2,0}, a);
    roots[0] = c_div(c_add(neg_b, sq), a2);
    roots[1] = c_div(c_sub(neg_b, sq), a2);
}

static Complex solve2(const Complex* args, int n) {
    if (n != 3) return (Complex){0, 0};
    if (args[0].real == 0 && args[0].imag == 0) return solve1(args + 1, 2);
    Complex roots[2];
    solve2_internal(args, roots);
    print_roots("solve2", roots, 2);
    return roots[0];
}

static void solve3_internal(const Complex* args, Complex* roots) {
    Complex A = args[0], B = args[1], C = args[2], D = args[3];
    Complex a = c_div(B, A), b = c_div(C, A), c = c_div(D, A);
    Complex a2 = c_mul(a, a);
    Complex p = c_sub(b, c_div(a2, (Complex){3,0}));
    Complex a3 = c_mul(a2, a);
    Complex term1 = c_div(c_mul((Complex){2,0}, a3), (Complex){27,0});
    Complex term2 = c_div(c_mul(a, b), (Complex){3,0});
    Complex q = c_add(c_sub(term1, term2), c);
    Complex q2 = c_mul(q, q);
    Complex p3 = c_mul(p, c_mul(p, p));
    Complex delta = c_add(c_div(q2, (Complex){4,0}), c_div(p3, (Complex){27,0}));
    Complex neg_q_2 = c_div(c_sub((Complex){0,0}, q), (Complex){2,0});
    Complex sq_delta = c_sqrt(delta);
    Complex u3_1 = c_add(neg_q_2, sq_delta);
    Complex u3_2 = c_sub(neg_q_2, sq_delta);
    Complex u_roots[3];
    c_cbrt_all(u3_1, u_roots);
    Complex a_3 = c_div(a, (Complex){3,0});
    for (int i = 0; i < 3; i++) {
        Complex u = u_roots[i];
        Complex v;
        if (u.real == 0 && u.imag == 0) {
            Complex v_roots[3];
            c_cbrt_all(u3_2, v_roots);
            v = v_roots[i];
        } else {
            v = c_div(c_sub((Complex){0,0}, p), c_mul((Complex){3,0}, u));
        }
        Complex y = c_add(u, v);
        roots[i] = c_sub(y, a_3);
    }
}

static Complex solve3(const Complex* args, int n) {
    if (n != 4) return (Complex){0, 0};
    if (args[0].real == 0 && args[0].imag == 0) return solve2(args + 1, 3);
    Complex roots[3];
    solve3_internal(args, roots);
    print_roots("solve3", roots, 3);
    return roots[0];
}

static Complex solve4(const Complex* args, int n) {
    if (n != 5) return (Complex){0, 0};
    if (args[0].real == 0 && args[0].imag == 0) return solve3(args + 1, 4);
    
    Complex A = args[0], B = args[1], C = args[2], D = args[3], E = args[4];
    Complex a = c_div(B, A), b = c_div(C, A), c = c_div(D, A), d = c_div(E, A);
    Complex a2 = c_mul(a, a);
    Complex a3 = c_mul(a2, a);
    Complex a4 = c_mul(a3, a);
    
    Complex p = c_sub(b, c_div(c_mul((Complex){3,0}, a2), (Complex){8,0}));
    Complex q = c_add(c_sub(c, c_div(c_mul(a, b), (Complex){2,0})), c_div(a3, (Complex){8,0}));
    
    Complex r_t1 = d;
    Complex r_t2 = c_div(c_mul(a, c), (Complex){4,0});
    Complex r_t3 = c_div(c_mul(a2, b), (Complex){16,0});
    Complex r_t4 = c_div(c_mul((Complex){3,0}, a4), (Complex){256,0});
    Complex r = c_sub(c_add(c_sub(r_t1, r_t2), r_t3), r_t4);
    
    Complex u_roots[4];
    if (fabs(q.real) < 1e-12 && fabs(q.imag) < 1e-12) {
        Complex p2 = c_mul(p, p);
        Complex r4 = c_mul((Complex){4,0}, r);
        Complex sq = c_sqrt(c_sub(p2, r4));
        Complex neg_p = c_sub((Complex){0,0}, p);
        Complex y1 = c_div(c_add(neg_p, sq), (Complex){2,0});
        Complex y2 = c_div(c_sub(neg_p, sq), (Complex){2,0});
        u_roots[0] = c_sqrt(y1);
        u_roots[1] = c_sub((Complex){0,0}, u_roots[0]);
        u_roots[2] = c_sqrt(y2);
        u_roots[3] = c_sub((Complex){0,0}, u_roots[2]);
    } else {
        Complex res_args[4];
        res_args[0] = (Complex){1,0};
        res_args[1] = c_sub((Complex){0,0}, p);
        res_args[2] = c_mul((Complex){-4,0}, r);
        res_args[3] = c_sub(c_mul((Complex){4,0}, c_mul(p, r)), c_mul(q, q));
        
        Complex res_roots[3];
        solve3_internal(res_args, res_roots);
        Complex y = res_roots[0];
        
        Complex y_min_p = c_sub(y, p);
        Complex S = c_sqrt(y_min_p);
        
        Complex y_2 = c_div(y, (Complex){2,0});
        Complex q_2S = c_div(q, c_mul((Complex){2,0}, S));
        
        Complex quad1[3] = {(Complex){1,0}, S, c_sub(y_2, q_2S)};
        solve2_internal(quad1, &u_roots[0]);
        
        Complex quad2[3] = {(Complex){1,0}, c_sub((Complex){0,0}, S), c_add(y_2, q_2S)};
        solve2_internal(quad2, &u_roots[2]);
    }
    
    Complex roots[4];
    Complex a_4 = c_div(a, (Complex){4,0});
    for (int i = 0; i < 4; i++) {
        roots[i] = c_sub(u_roots[i], a_4);
    }
    print_roots("solve4", roots, 4);
    return roots[0];
}

// --- Tabla de callbacks ---
static FuncEntry func_table[] = {
    {"sin",   1, f_sin,   NULL, NULL, NULL},
    {"cos",   1, f_cos,   NULL, NULL, NULL},
    {"tan",   1, f_tan,   NULL, NULL, NULL},
    {"asin",  1, f_asin,  NULL, NULL, NULL},
    {"acos",  1, f_acos,  NULL, NULL, NULL},
    {"atan",  1, f_atan,  NULL, NULL, NULL},
    {"atan2", 2, NULL,    f_atan2, NULL, NULL},
    {"ln",    1, f_ln,    NULL, NULL, NULL},
    {"log",   2, NULL,    f_log_base, NULL, NULL},
    {"log2",  1, f_log2,  NULL, NULL, NULL},
    {"log10", 1, f_log10, NULL, NULL, NULL},
    {"exp",   1, exp,     NULL, NULL, NULL},
    {"sqrt",  1, f_sqrt,  NULL, NULL, NULL},
    {"abs",   1, fabs,    NULL, NULL, NULL},
    {"max",   2, NULL,    f_max, NULL, NULL},
    {"min",   2, NULL,    f_min, NULL, NULL},
    {"pow",   2, NULL,    pow, NULL, NULL},
    {"solve1", 2, NULL,   NULL, NULL, solve1},
    {"solve2", 3, NULL,   NULL, NULL, solve2},
    {"solve3", 4, NULL,   NULL, NULL, solve3},
    {"solve4", 5, NULL,   NULL, NULL, solve4}
};
static int num_functions = sizeof(func_table) / sizeof(func_table[0]);

static const char* func_descriptions[] = {
    "Seno", "Coseno", "Tangente", "Arcoseno              (dominio: [-1, 1])",
    "Arcocoseno            (dominio: [-1, 1])", "Arcotangente", "Arcotangente de 2 args (atan2(y, x))",
    "Logaritmo natural     (base e, x > 0)", "Logaritmo en base a de b (a > 0, a != 1, b > 0)",
    "Logaritmo base 2      (x > 0)", "Logaritmo base 10     (x > 0)", "Exponencial           (e^x)",
    "Raiz cuadrada         (x >= 0)", "Valor absoluto", "Maximo entre dos valores", "Minimo entre dos valores",
    "Potencia              (equivale a a^b)", 
    "Resuelve ecuacion lineal ax + b = 0", "Resuelve ecuacion cuadratica ax^2 + bx + c = 0",
    "Resuelve ecuacion cubica ax^3 + bx^2 + cx + d = 0", "Resuelve ecuacion cuartica ax^4 + bx^3 + cx^2 + dx + e = 0"
};

void list_functions(void) {
    printf("Funciones matematicas disponibles:\n");
    printf("  %-26s  %s\n", "Llamada", "Descripcion");
    printf("  %-26s  %s\n", "--------------------------", "------------------------------------");
    for (int i = 0; i < num_functions; i++) {
        char signature[40];
        if (func_table[i].num_args == 1) {
            snprintf(signature, sizeof(signature), "%s(x)", func_table[i].name);
        } else if (func_table[i].num_args == 2) {
            snprintf(signature, sizeof(signature), "%s(a, b)", func_table[i].name);
        } else if (func_table[i].num_args == 3) {
            snprintf(signature, sizeof(signature), "%s(a, b, c)", func_table[i].name);
        } else if (func_table[i].num_args == 4) {
            snprintf(signature, sizeof(signature), "%s(a, b, c, d)", func_table[i].name);
        } else if (func_table[i].num_args == 5) {
            snprintf(signature, sizeof(signature), "%s(a,b,c,d,e)", func_table[i].name);
        } else {
            snprintf(signature, sizeof(signature), "%s(x1, ...)", func_table[i].name);
        }
        printf("  %-26s  -> %s\n", signature, func_descriptions[i]);
    }
    printf("\nConstantes disponibles:\n");
    printf("  %-26s  -> %s\n", "pi",  "3.14159265358979...  (constante)");
    printf("  %-26s  -> %s\n", "e",   "2.71828182845904...  (constante)");
    printf("  %-26s  -> %s\n", "phi", "1.61803398874989...  (constante, numero aureo)");
    printf("  %-26s  -> %s\n", "i",   "0 + 1i               (constante imaginaria)");
    printf("\nOperadores disponibles:\n");
    printf("  +  Suma          -  Resta\n");
    printf("  *  Multiplicar   /  Dividir\n");
    printf("  ^  Potencia      %%  Modulo\n");
}

int call_function(const char* name, const Complex* args, int num_args, Complex* out_result) {
    for (int i = 0; i < num_functions; i++) {
        if (strcmp(func_table[i].name, name) == 0) {
            if (func_table[i].num_args != -1 && func_table[i].num_args != num_args) {
                printf("Error: '%s' espera %d argumento(s), pero se dieron %d.\n", name, func_table[i].num_args, num_args);
                return 0; // Error de invocacion
            }
            if (func_table[i].funcNC) {
                *out_result = func_table[i].funcNC(args, num_args);
            } else if (func_table[i].funcN) {
                if (num_args == 0) {
                    printf("Error: '%s' espera al menos 1 argumento.\n", name);
                    return 0;
                }
                double res = func_table[i].funcN(args, num_args);
                *out_result = (Complex){res, 0.0};
            } else if (num_args == 1) {
                double res = func_table[i].func1(args[0].real);
                *out_result = (Complex){res, 0.0};
            } else if (num_args == 2) {
                double res = func_table[i].func2(args[0].real, args[1].real);
                *out_result = (Complex){res, 0.0};
            }
            return 1; // Exitoso
        }
    }
    printf("Error: Funcion desconocida '%s'\n", name);
    return 0;
}

// --- Rutinas Estadisticas Interactivas ---
static int cmp_double(const void* a, const void* b) {
    double arg1 = *(const double*)a;
    double arg2 = *(const double*)b;
    if (arg1 < arg2) return -1;
    if (arg1 > arg2) return 1;
    return 0;
}

void stat_calc_discrete(double* data, int n) {
    if (n <= 0) return;
    double sum = 0;
    for (int i = 0; i < n; i++) sum += data[i];
    double mean = sum / n;
    
    // Sort para mediana, moda, y cuartiles
    double* sorted = (double*)malloc(n * sizeof(double));
    memcpy(sorted, data, n * sizeof(double));
    qsort(sorted, n, sizeof(double), cmp_double);
    
    double median = (n % 2 == 0) ? (sorted[n/2 - 1] + sorted[n/2]) / 2.0 : sorted[n/2];
    
    double mode = sorted[0];
    int max_count = 1, current_count = 1;
    for (int i = 1; i < n; i++) {
        if (sorted[i] == sorted[i-1]) current_count++;
        else {
            if (current_count > max_count) { max_count = current_count; mode = sorted[i-1]; }
            current_count = 1;
        }
    }
    if (current_count > max_count) mode = sorted[n-1];
    
    double sum_sq_diff = 0;
    for (int i = 0; i < n; i++) {
        double diff = data[i] - mean;
        sum_sq_diff += diff * diff;
    }
    double var = sum_sq_diff / n;
    double stddev = sqrt(var);
    
    // Cuartiles
    double q1_pos = 0.25 * (n - 1);
    int q1_idx = (int)q1_pos;
    double q1 = (q1_idx + 1 < n) ? sorted[q1_idx] + (q1_pos - q1_idx)*(sorted[q1_idx+1] - sorted[q1_idx]) : sorted[q1_idx];
    
    double q3_pos = 0.75 * (n - 1);
    int q3_idx = (int)q3_pos;
    double q3 = (q3_idx + 1 < n) ? sorted[q3_idx] + (q3_pos - q3_idx)*(sorted[q3_idx+1] - sorted[q3_idx]) : sorted[q3_idx];
    
    printf("\n--- Resultados (Variable Discreta) ---\n");
    printf("Media               = %g\n", mean);
    printf("Mediana             = %g\n", median);
    printf("Moda                = %g\n", mode);
    printf("Varianza            = %g\n", var);
    printf("Desviacion estandar = %g\n", stddev);
    printf("Primer Cuartil (Q1) = %g\n", q1);
    printf("Tercer Cuartil (Q3) = %g\n", q3);
    printf("--------------------------------------\n");
    
    free(sorted);
}

void stat_calc_continuous(double* lim_inf, double* lim_sup, double* freqs, int n) {
    if (n <= 0) return;
    double total_n = 0;
    for (int i = 0; i < n; i++) total_n += freqs[i];
    if (total_n == 0) return;
    
    double* xi = (double*)malloc(n * sizeof(double));
    double* F = (double*)malloc(n * sizeof(double));
    
    double sum = 0;
    double current_F = 0;
    for (int i = 0; i < n; i++) {
        xi[i] = (lim_inf[i] + lim_sup[i]) / 2.0;
        sum += xi[i] * freqs[i];
        current_F += freqs[i];
        F[i] = current_F;
    }
    double mean = sum / total_n;
    
    // Varianza
    double sum_sq_diff = 0;
    for (int i = 0; i < n; i++) {
        double diff = xi[i] - mean;
        sum_sq_diff += freqs[i] * diff * diff;
    }
    double var = sum_sq_diff / total_n;
    double stddev = sqrt(var);
    
    // Mediana
    double n_2 = total_n / 2.0;
    double median = 0;
    for (int i = 0; i < n; i++) {
        if (F[i] >= n_2) {
            double F_prev = (i == 0) ? 0 : F[i-1];
            double A = lim_sup[i] - lim_inf[i];
            median = lim_inf[i] + ((n_2 - F_prev) / freqs[i]) * A;
            break;
        }
    }
    
    // Moda
    double mode = 0;
    int max_f_idx = 0;
    for (int i = 1; i < n; i++) {
        if (freqs[i] > freqs[max_f_idx]) max_f_idx = i;
    }
    double d1 = freqs[max_f_idx] - ((max_f_idx == 0) ? 0 : freqs[max_f_idx-1]);
    double d2 = freqs[max_f_idx] - ((max_f_idx == n-1) ? 0 : freqs[max_f_idx+1]);
    if (d1 + d2 == 0) {
        mode = xi[max_f_idx]; // fallback if all freqs are same
    } else {
        double A = lim_sup[max_f_idx] - lim_inf[max_f_idx];
        mode = lim_inf[max_f_idx] + (d1 / (d1 + d2)) * A;
    }
    
    // Q1 y Q3
    double q1_pos = total_n / 4.0;
    double q1 = 0;
    for (int i = 0; i < n; i++) {
        if (F[i] >= q1_pos) {
            double F_prev = (i == 0) ? 0 : F[i-1];
            double A = lim_sup[i] - lim_inf[i];
            q1 = lim_inf[i] + ((q1_pos - F_prev) / freqs[i]) * A;
            break;
        }
    }
    
    double q3_pos = 3.0 * total_n / 4.0;
    double q3 = 0;
    for (int i = 0; i < n; i++) {
        if (F[i] >= q3_pos) {
            double F_prev = (i == 0) ? 0 : F[i-1];
            double A = lim_sup[i] - lim_inf[i];
            q3 = lim_inf[i] + ((q3_pos - F_prev) / freqs[i]) * A;
            break;
        }
    }
    
    // Imprimir Tabla de Frecuencias
    printf("\nTabla de Frecuencias:\n");
    printf("%-20s | %-16s | %-14s | %-15s | %-14s\n", "Limites", "Marca clase (xi)", "Frec. abs (fi)", "Frec. acum (Fi)", "Frec. rel (hi)");
    printf("---------------------|------------------|----------------|-----------------|----------------\n");
    for (int i = 0; i < n; i++) {
        char limites_str[40];
        snprintf(limites_str, sizeof(limites_str), "[%g - %g>", lim_inf[i], lim_sup[i]);
        double hi = freqs[i] / total_n;
        printf("%-20s | %-16.3f | %-14g | %-15g | %-14.3f\n", limites_str, xi[i], freqs[i], F[i], hi);
    }
    
    printf("\n--- Resultados (Variable Continua) ---\n");
    printf("Media               = %g\n", mean);
    printf("Mediana             = %g\n", median);
    printf("Moda                = %g\n", mode);
    printf("Varianza            = %g\n", var);
    printf("Desviacion estandar = %g\n", stddev);
    printf("Primer Cuartil (Q1) = %g\n", q1);
    printf("Tercer Cuartil (Q3) = %g\n", q3);
    printf("--------------------------------------\n");
    
    free(xi);
    free(F);
}
