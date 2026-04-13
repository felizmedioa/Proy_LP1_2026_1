# Entregable 1: Análisis y Diseño

## Requerimientos

### Funcionales
1. **Evaluación Aritmética Dinámica**: Evaluar operaciones matemáticas elementales (Suma `+`, Resta `-`, Multiplicación `*`, División `/`, Módulo `%`, Potencia `^`).
2. **Soporte de Variables**: Insertar y almacenar variables usando el operador de asignación `=` (ej. `a = x + y`), con capacidad a ser reutilizadas en la misma sesión interactiva.
3. **Manejo de Funciones Complejas**: Soportar funciones matemáticas integradas como exponenciales (`exp`, `sqrt`, `pow`), logarítmicas (`log`, `log2`, `log10`), trigonométricas (`sin`, `cos`, `tan`, `asin`, `acos`, `atan`, `atan2`) y de rango discreto (`max`, `min`, `abs`).
4. **Resguardo de Excepciones**: Validación y manejo seguro de división por cero o errores de dominio (por ejemplo, `log(-1)`). Las métricas erróneas deben advertirse en lugar de colgar ("crashear") la aplicación.
5. **Configuración de Modos**: Alterar la unidad angular evaluada en funciones trigonométricas (mediante comandos locales para Radianes o Grados).
6. **Entorno REPL**: Ciclo continuo que toma la entrada del usuario, procesa mediante sintaxis abstracta e imprime el resultado al vuelo en consola. Opciones de utilidad rápida como `list` para el volcado de diccionarios/memoria y `quit` o `exit`.

### No Funcionales
1. **Restricción de Bibliotecas**: Desplegar el sistema usando la biblioteca ANSI C nativa, exclusivamente dependiendo de estándares base como `<stdio.h>`, `<stdlib.h>`, `<string.h>` y `<math.h>`, excluyendo dependencias a paquetes externos pesados.
2. **Arquitectura Modulada**: Implementar explícita separación de lógicas y separación limpia del Lexer, Parser, Estructuras Abstractas y CLI main para un marco fácilmente extensible, mantenible y sujeto a testeos exhaustivos.

---

## Gramática Formal (EBNF)

A continuación, exponemos las reglas formales de precedencia sobre las cuáles nuestro parser descendente recursivo opera:

```ebnf
Expression   ::= Assign
Assign       ::= IDENTIFIER "=" Assign | AddSub
AddSub       ::= MulDivMod { ("+" | "-") MulDivMod }
MulDivMod    ::= Power { ("*" | "/" | "%") Power }
Power        ::= Unary { "^" Power }
Unary        ::= ("+" | "-") Unary | Primary
Primary      ::= NUMBER 
               | "(" Assign ")"
               | IDENTIFIER
               | IDENTIFIER "(" [ Assign { "," Assign } ] ")"
```
*(Nota: la regla de potencia y de asignación se asocian típicamente hacia la derecha)*

---

## Diagrama de Flujo del Pipeline

El ciclo de recolección atraviesa las siguientes barreras estrictas en nuestro motor `eval_math`:

1. **Lectura (String en C)**: Se procesa usando `fgets(..., stdin)` obteniendo texto plano.
2. **Tokenización (`lexer.c`)**: Se invoca a `get_next_token()` y los caracteres secuenciales se aglomeran en bloques o *Tokens* (ej. Un `"l"`, `"o"` y `"g"` pegados se convierten en un Token tipo `TOK_IDENTIFIER("log")`). Descarta los espacios de por medio.
3. **Análisis Sintáctico (`parser.c`)**: Llama continuadamente a los *tokens*. Basado en las reglas EBNF (Precedencia), inserta nodos asimétricos hijos en las profundidades correctas (ej. la Multiplicación anidada abajo de la Suma madre). Nos da a cambio la raíz de un Árbol (`ASTNode*`).
4. **Evaluador de Árbol Sintáctico (`eval.c`)**: Desciende por el AST (empezando de lo más profundo que sean nudos de variables `symtable.c` o literales, y yendo hacia arriba) y consolida la cascada hasta devolver el `double` flotante equivalente para el nodo de la raíz madre.
5. **Print Loop**: El proceso final imprime la doble variable en consola o lanza el warning correspondiente en fallos.
