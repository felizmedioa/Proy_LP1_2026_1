# Proyecto 11 - Mini evaluador de expresiones algebraicas

## Descripción del Proyecto
Este proyecto es un evaluador interactivo de expresiones algebraicas escrito en lenguaje C (sin dependencias externas pesadas, salvo la librería matemática estándar `-lm`). Funciona como un intérprete tipo REPL (Read-Eval-Print Loop) inspirado en calculadoras o herramientas como GeoGebra. Soporta operadores básicos de aritmética, funciones trigonométricas, logarítmicas, variables al vuelo y manejo completo de la procedencia y asociatividad matemática a través de un Árbol de Sintaxis Abstracta (AST) derivado de un Analizador Léxico (Lexer) y Sintáctico (Parser) de descenso recursivo construidos desde cero.

## Lista de Integrantes
- [Rodrigo Matias Ortiz Montoya]
- [Piero Mauro Morales Sinche]
- [Aldair Edgard Gutierrez Mamani]
- [Jose Luis Canchanya Ramos]

## Estructura del Proyecto
```text
Proy_LP1_2026_1/
├── docs/                      # Documentación del proyecto
├── entregable1/               # Archivos del primer entregable
├── entregable2/               # Archivos del segundo entregable
├── RequerimientosIniciales/   # Documentos de requerimientos iniciales
├── eval.c / eval.h            # Evaluación del Árbol de Sintaxis Abstracta (AST)
├── functions.c / functions.h  # Implementación de funciones matemáticas (sin, cos, etc.)
├── lexer.c / lexer.h          # Analizador léxico (Tokenizador)
├── main.c                     # Punto de entrada y ciclo REPL interactivo
├── parser.c / parser.h        # Analizador sintáctico (Construcción del AST)
├── symtable.c / symtable.h    # Tabla de símbolos para variables y constantes
├── Makefile                   # Reglas de compilación (Make)
├── test_input*.txt            # Archivos de texto con entradas de prueba
└── README.md                  # Este archivo de documentación
```

## Características Principales
*  Operadores: `+`, `-`, `*`, `/`, `^` (potencia).
*  Funciones: `sin`, `cos`, `tan`, `sqrt`, `log`, `log10`, `abs`, `round`, `ceil`, `floor`, `max`, `min`.
*  Constantes: `pi`, `e`.
*  Variables: Definición y uso (ej: `x = 5`).
*  Unidades: Soporta cambio entre Radianes y Grados (`rad` / `deg`).
*  Manejo de errores: Detección de errores léxicos, sintácticos y de dominio (ej: `sqrt(-1)`).

## Beneficios de la aplicación
1. **Velocidad y Ligereza**: Al estar implementado puramente en C nativo, la sobrecarga es casi nula, permitiendo la evaluación instantánea de operaciones complejas.
2. **Interfaz Interactiva**: Los comandos amigables y el formato limpio a través de consola lo hacen útil en computadoras con pocos recursos o para uso por SSH.
3. **Persistencia de Variables**: Permite asignar valores temporalmente (`x = 5 + pi`) y reutilizarlos dinámicamente en futuros cómputos.
4. **Validación Exhaustiva de Errores**: Previene cierres inesperados gestionando activamente las divisiones por cero, dominios inválidos de números (raíz de negativos), y errores sintácticos.
5. **Configurable**: Posibilidad de cambiar entre unidades de ángulos en cualquier instante del uso (usar comando `deg` o `rad`).

## Instalación y Configuración del Entorno (Windows)

Para compilar el proyecto utilizando el archivo `Makefile` provisto, el sistema requiere tener instalado **Make**.

1. **Instalar Chocolatey:** Abre PowerShell como Administrador y corre el comando de instalación oficial (revisar [Documentación de Chocolatey](https://chocolatey.org/install#individual)):
   ```powershell
   Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
   ```
2. **Instalar Make:** Una vez instalado Chocolatey, en la misma consola como administrador, ejecuta el siguiente comando (referencia: [StackOverflow](https://stackoverflow.com/questions/32127524/how-can-i-install-and-use-make-in-windows)):
   ```powershell
   choco install make
   ```
3. Reinicia tu terminal o Visual Studio Code para que reconozca los cambios en las variables de entorno.

## Uso Básico

Una vez cuentes con `make` (y tu compilador base como `gcc` a través de MinGW), navega por terminal a la carpeta de este proyecto y compila:
```bash
make
./eval_math
```

Dentro del REPL interactivo puedes tipear comandos y evaluarlos:
- `asin(sin(pi/2))`
- `a = max(10, 5)`
- `list` (para visualizar las variables guardadas)
- `quit` (o `exit`, para salir del programa)
