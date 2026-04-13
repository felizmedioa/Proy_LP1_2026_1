# Entregable 3: Casos de Prueba y Ejemplos de uso

A continuación, listaremos baterías de casos para testear que la interacción local cumpla con las especificaciones del Evaluador en el Entregable 1.

### Manejo de Fallos (Protección ante cierres)
Buscamos que estas simulaciones no rompan o cuelguen la terminal:

| Entrada Ingresada por Sesión      | Salida Proyectada Requerida                                | ¿Logrado? | Razón                              |
|------------------------------------|-------------------------------------------------------------|-----------|-------------------------------------|
| `a = 5 / 0`                       | _Error matemático: División por cero_                             | Sí        | Condicionado explícitamente en AST  |
| `log(-5)`                         | _Error de Dominio: log(-5) negativo o cero_                       | Sí        | Validación wrapper en `f_log`    |
| `asin(2.5)`                       | _Error de Dominio: asin(2.5) fuera del rango [-1, 1]._        | Sí        | Validación wrapper en `f_asin`    |
| `(5 + )`                          | _Error de sintaxis: Expresión o símbolo inválido esperado_        | Sí        | El parser de factor no encuentra dato|
| `5 * 4)`                          | _Error de sintaxis: Fueron encontrados símbolos inesperados..._   | Sí        | Paréntesis de cierre sobrante interceptado del búfer de tokens |

---

### Casos de Prueba Directos - (Equivalentes de Consola)

Comparativa esperada asumiendo ejecuciones de 10 ejemplos con resultados iguales a los de su versión original en los motores pesados GeoGebra locales.

1. **Aritmética lineal y Procedencia Normal**
   - **Ingreso**: `5 + 3 * 2`
   - **Salida**: `11` *(En vez de 16, respetando la multiplicación primero)*.

2. **Uso de asociatividad derecha controlada**
   - **Ingreso**: `2 ^ 3 ^ 2`
   - **Salida**: `512` *(Asociable 2^9 y no 8^2)*.

3. **Constantes precargadas dinámicas**
   - **Ingreso**: `log(e)`
   - **Salida**: `1`

4. **Variables temporales apiladas**
   - **Ingreso**: `x = 5`, seguido de `y = x + 5`, luego evaluar `y * 2`.
   - **Salida**: Devolución de estado `20`.

5. **Resoluciones Trigonométricas (Por defecto Radianes)**
   - **Ingreso**: `sin(pi/2)`
   - **Salida**: `1`

6. **Desplazamiento angular (Modo Grados "deg" activo)**
   - **Ingreso**: *(Después de colocar `deg`)* `sin(90)`
   - **Salida**: `1`

7. **Sobrecarga de parámetros y arreglos min/max**
   - **Ingreso**: `max(10 - 2, 5 * 2)`
   - **Salida**: `10`

8. **Inclusión de Unary minuses**
   - **Ingreso**: `-5 + 10`
   - **Salida**: `5`

9. **Módulo mixto flotante/entero (fmod)**
   - **Ingreso**: `10.5 % 3`
   - **Salida**: `1.5`

10. **Composición densa general**
    - **Ingreso**: `sqrt(pow(3, 2) + pow(4, 2))`
    - **Salida**: `5`
