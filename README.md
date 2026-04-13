# Proyecto 11 - Mini evaluador de expresiones algebraicas

## Descripción del Proyecto
Este proyecto es un evaluador interactivo de expresiones algebraicas escrito en lenguaje C (sin dependencias externas pesadas, salvo la librería matemática estándar `-lm`). Funciona como un intérprete tipo REPL (Read-Eval-Print Loop) inspirado en calculadoras o herramientas como GeoGebra. Soporta operadores básicos de aritmética, funciones trigonométricas, logarítmicas, variables al vuelo y manejo completo de la procedencia y asociatividad matemática a través de un Árbol de Sintaxis Abstracta (AST) derivado de un Analizador Léxico (Lexer) y Sintáctico (Parser) de descenso recursivo construidos desde cero.

## Beneficios de la aplicación
1. **Velocidad y Ligereza**: Al estar implementado puramente en C nativo, la sobrecarga es casi nula, permitiendo la evaluación instantánea de operaciones complejas.
2. **Interfaz Interactiva**: Los comandos amigables y el formato limpio a través de consola lo hacen útil en computadoras con pocos recursos o para uso por SSH.
3. **Persistencia de Variables**: Permite asignar valores temporalmente (`x = 5 + pi`) y reutilizarlos dinámicamente en futuros cómputos.
4. **Validación Exhaustiva de Errores**: Previene cierres inesperados gestionando activamente las divisiones por cero, dominios inválidos de números (raíz de negativos), y errores sintácticos.
5. **Configurable**: Posibilidad de cambiar entre unidades de ángulos en cualquier instante del uso (usar comando `deg` o `rad`).

## Uso Básico
Para compilar en Linux o Windows (con herramientas GNU o MinGW):
```bash
make
./eval_math
```

Dentro del REPL puedes tipear comandos como:
- `asin(sin(pi/2))`
- `a = max(10, 5)`
- `list` (para ver las variables guardadas)
- `quit` (para salir)
