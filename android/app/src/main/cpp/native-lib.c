#include <jni.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>

#include "parser.h"
#include "eval.h"
#include "symtable.h"
#include "functions.h"

// Custom printf implementation to capture outputs
char g_output_buffer[32768];
int g_output_pos = 0;

int my_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    int remaining = sizeof(g_output_buffer) - g_output_pos - 1;
    if (remaining > 0) {
        int written = vsnprintf(g_output_buffer + g_output_pos, remaining, format, args);
        if (written > 0) {
            g_output_pos += written;
            if (g_output_pos >= sizeof(g_output_buffer)) {
                g_output_pos = sizeof(g_output_buffer) - 1;
            }
        }
    }
    va_end(args);
    return 0; // return status
}

JNIEXPORT void JNICALL
Java_com_example_expressionevaluator_MainActivity_initEvaluator(JNIEnv *env, jobject thiz) {
    symtable_init();
}

JNIEXPORT void JNICALL
Java_com_example_expressionevaluator_MainActivity_setAngleUnit(JNIEnv *env, jobject thiz, jint mode) {
    set_angle_unit(mode);
}

JNIEXPORT jint JNICALL
Java_com_example_expressionevaluator_MainActivity_getAngleUnit(JNIEnv *env, jobject thiz) {
    return get_angle_unit();
}

JNIEXPORT jstring JNICALL
Java_com_example_expressionevaluator_MainActivity_evaluateExpression(JNIEnv *env, jobject thiz, jstring j_expression) {
    // Clear buffer
    g_output_pos = 0;
    g_output_buffer[0] = '\0';

    const char *expression = (*env)->GetStringUTFChars(env, j_expression, NULL);
    if (!expression) {
        return (*env)->NewStringUTF(env, "Error: Entrada nula.");
    }

    // Process quick commands
    if (strcmp(expression, "list") == 0) {
        symtable_list();
    } else if (strcmp(expression, "funct") == 0) {
        list_functions();
    } else if (strcmp(expression, "deg") == 0) {
        set_angle_unit(1);
        my_printf("Modo angular cambiado a GRADOS.\n");
    } else if (strcmp(expression, "rad") == 0) {
        set_angle_unit(0);
        my_printf("Modo angular cambiado a RADIANES.\n");
    } else {
        // Standard expression evaluation
        init_lexer(expression);
        parse_error = 0;
        ASTNode* ast = parse_expression();

        if (ast && !parse_error) {
            eval_error = 0;
            Complex result = eval_ast(ast);

            if (!eval_error) {
                // Truncate tiny values due to floating-point precision
                if (fabs(result.real) < 1e-15) result.real = 0.0;
                if (fabs(result.imag) < 1e-15) result.imag = 0.0;

                if (result.imag == 0.0) {
                    my_printf("%g\n", result.real);
                } else if (result.real == 0.0) {
                    if (result.imag == 1.0) my_printf("i\n");
                    else if (result.imag == -1.0) my_printf("-i\n");
                    else my_printf("%gi\n", result.imag);
                } else {
                    if (result.imag > 0) {
                        if (result.imag == 1.0) my_printf("%g + i\n", result.real);
                        else my_printf("%g + %gi\n", result.real, result.imag);
                    } else {
                        if (result.imag == -1.0) my_printf("%g - i\n", result.real);
                        else my_printf("%g - %gi\n", result.real, fabs(result.imag));
                    }
                }
            }
            free_ast(ast);
        }
        // If there was parse_error or eval_error, the printfs inside parse/eval have already written to g_output_buffer
    }

    (*env)->ReleaseStringUTFChars(env, j_expression, expression);

    return (*env)->NewStringUTF(env, g_output_buffer);
}
