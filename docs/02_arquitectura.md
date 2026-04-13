# Entregable 2: Arquitectura del Sistema

## 1. Diseño del Árbol de Sintaxis Abstracta (AST)

En el fichero abstracto del intérprete, se definió un nodo estructural unificado (un mega-struct en C dictaminado en `parser.h`). 
Este modelo jerárquico representa las relaciones lógicas de los tokens. Cada rama o `ASTNode` puede adoptar una personalidad exclusiva a través de su propiedad interna `ASTNodeType`:

- `AST_NUMBER`: Hoja final del árbol. Almacena de lado simple la variable `value = flotante` de un número.
- `AST_VAR`: Hoja final abstracta. Carga consigo el `name` a ser extraído en tiempo de análisis desde la red variable global.
- `AST_BINOP`: Operador estándar. Requiere tener mapeada a `left` (nodo izquierdo) y `right` (nodo derecho) obligatoriamente.
- `AST_UNOP`: Operador solitario (ej. un menos a la izquierda de un número). Modifica su nodo `left` y nada más.
- `AST_FUNC`: Contenedor versátil. Emplea un arreglo estático `args` que almacena un puente múltiple de expresiones que se le insertaron para su resolución.
- `AST_ASSIGN`: Nodo inyector. El puntero `name` es declarado con los valores resueltos del puntero `right`.

El AST emplea llamadas de "destrucción en cascada" en `parser.c` con `free_ast(root)` en las limpiezas de memoria tras usarse, evitando memory leaks en compiladores sin recolectores automáticos como GCC tradicional.

## 2. Tabla de Funciones (`functions.c`)

El enfoque tradicional en intérpretes primerizos sería reventar la zona matemática a puros sentencias interminables `if/else`. Para conseguir una arquitectura limpia decidimos alojar nuestra lógica en un "Contenedor Array C": la **Tabla de Callbacks de Funciones**. 

1. Se unificaron `typedef` funcionales que emulan prototipos (`double (*MathFunc1)(double)` y `double (*MathFunc2)(double, double)`).
2. Se construyó el arreglo contenedor en caliente indexado:
```c
static FuncEntry func_table[] = {
    {"sin", 1, f_sin, NULL},
    {"cos", 1, f_cos, NULL}, ...
    {"atan2", 2, NULL, f_atan2},
    {"log", 1, f_log, NULL}, ...
```
3. El motor despachador interno `call_function()` simplemente itera al momento de encontrarse un `AST_FUNC`. Localiza el nombre emparejando el string, revisa si hay coherencia estricta en el nivel de argumentos requeridos, e inyecta dinámicamente dichos parámetros llamando a la función estandarizada oculta mediante puntero. Esto hace al engine extensible a 30 o incluso 100 operativas nuevas alterando literalmente 1 línea de código en un arreglo.

## 3. Tabla de Símbolos (`symtable.c`)

Basándonos de nuevo en el poder de la simpleza. Mantenemos el estado local alojado tras una macro `MAX_VARIABLES` seteada en un arreglo contiguo estático con structuras `{ char name[32]; double value; }`. 
El módulo proporciona las interfaces `symtable_set(...)` para buscar/sustituir o agregar dependencias con un for lineal (fácil procesamiento dada la escala). Se exponen utilidades complementarias a un macro comando (`symtable_list`) para volcar estado al Main (el requerimiento de GeoGebra Style con un comando `list` en vivo). 

## 4. Estrategias de Tolerancia de Errores Matemáticos Genuinos

Inyectamos variables de bandera `eval_error` e impedimos la propagación de anomalías como divisiones entre 0 `x / 0.0`. En la tabla de funciones personalizadas (eg. dentro del wrapper `f_asin` nativo creado), se valida por delante el dominio antes de invocar la librería real originaria de `math.h`. Si la función requiere $[-1, 1]$ pero recibe un `100` , se imprime un advertimento amigable a la consola interactiva por stdout, se infla la bandera de evaluación y se descarta todo fallo de sistema salvando la vida del sistema en vez del clásico `MathError: program shutdown`.
