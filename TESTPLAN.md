# TESTPLAN — Casos de prueba del proyecto

Este documento lista todos los casos de prueba del proyecto y cómo ejecutarlos.

Hay dos suites automatizadas:

| Suite | Archivo | Qué prueba | Checks | Ejecución |
|---|---|---|---|---|
| C++ (caja negra) | `tests/test_program.py` | El ejecutable `build/SistemasDinamicos.exe`: clasificación, mensajes, CSV, PNG | 37 | `.venv\Scripts\python.exe tests\test_program.py` |
| Web (e2e) | `tests/test_app_e2e.py` | La interfaz `app.py` vía Streamlit AppTest: informe, gráfica, HTML | 19 | `.venv\Scripts\python.exe tests\test_app_e2e.py` |

Los 37 checks de la suite C++ se componen de 35 casos (T01–T29c, T30, 2 focos, 1 no lineal) más 2 verificaciones globales (formato del CSV y regeneración del PNG).

---

## 1. Suite C++ — los 37 checks

### 1.1 Casos lineales (modo "Sistema lineal" de la app web)

En el sidebar de la web los campos se ven en dos columnas: `a1` / `a2` a la izquierda y `b1` / `b2` a la derecha. El orden de la tabla es ese mismo.

| Caso | a1 | a2 | b1 | b2 | Resultado esperado (web) |
|---|---|---|---|---|---|
| T01 estrella | 1 | 0 | 0 | 1 | Nodo estrella · Inestable |
| T02 nodo estable | -1 | 0 | 0 | -1 | Nodo · Asintóticamente estable |
| T03 centro | 0 | -1 | 1 | 0 | Centro · Estable (no asintótico) |
| T04 centro opuesto | 0 | 1 | -1 | 0 | Centro · Estable (no asintótico) |
| T05 silla | 1 | 0 | 0 | -1 | Punto silla · Inestable |
| T06 silla 2 | 0 | 1 | 1 | 0 | Punto silla · Inestable |
| T07 det=0 | 0 | 0 | 1 | 0 | NO ES UNICO, sin gráfica |
| T08 filas prop. | 1 | 1 | 1 | 1 | NO ES UNICO, sin gráfica |
| T09 matriz nula | 0 | 0 | 0 | 0 | NO ES UNICO, sin gráfica |
| T10 det=0 | 1 | 0 | 0 | 0 | NO ES UNICO, sin gráfica |
| T11 det=0 | 0 | 1 | 0 | 1 | NO ES UNICO, sin gráfica |
| T12 det=0 | 1 | 0 | 1 | 0 | NO ES UNICO, sin gráfica |
| T13 det exacto | 1 | 2 | 2 | 4 | NO ES UNICO, sin gráfica |
| T14 filas prop. | 1 | 3 | 2 | 6 | NO ES UNICO, sin gráfica |
| T15 nodo inest. | 1 | 0 | 0 | 2 | Nodo · Inestable |
| T16 nodo est. | -1 | 0 | 0 | -2 | Nodo · Asintóticamente estable |
| T17 foco inest. | 1 | -1 | 1 | 1 | Foco (espiral) · Inestable |
| T18 foco est. | -1 | -1 | 1 | -1 | Foco (espiral) · Asintóticamente estable |
| T19 impropio | 2 | 0 | 1 | 2 | Nodo impropio · Inestable |
| T20 impropio est. | -2 | 0 | 1 | -2 | Nodo impropio · Asintóticamente estable |
| T21 det=0 | 0 | 1 | 0 | 0 | NO ES UNICO, sin gráfica |
| T22 casi degenerado | 1 | 0 | 0 | 1e-12 | Nodo · Inestable |
| T23 det~1e-10 | 1 | 1 | 1 | 1.0000000001 | Nodo · Inestable |
| T24 det=0 en FP | 0.1 | 0.3 | 0.2 | 0.6 | NO ES UNICO, sin gráfica |
| T25 overflow | 1e6 | 1e6 | 1e6 | 1e6 | NO ES UNICO, sin gráfica |
| T26 sin overflow | 2147483647 | 1 | 1 | 2147483647 | Nodo · Inestable |
| T30 centro c/orbitas | 0 | -1 | 1 | 0 | Centro · Estable (no asintótico) |
| Foco estable | -0.5 | 1 | -1 | -0.5 | Foco (espiral) · Asintóticamente estable |
| Foco inestable | 0.5 | 1 | -1 | 0.5 | Foco (espiral) · Inestable |

### 1.2 Caso no lineal (modo "Sistema no lineal" de la app web)

| Ecuación | a | b | c | d | e |
|---|---|---|---|---|---|
| x' | -1 | -1 | 1 | 0 | 0 |
| y' | 1 | -1 | 0 | 0 | 0 |

Resultado esperado: Foco (espiral) · Asintóticamente estable + `Teorema de Hartman-Grobman APLICA`. Es el caso que ya viene por defecto al cambiar a modo no lineal.

### 1.3 Casos solo CLI (no se pueden teclear en la web)

El formulario de la web solo acepta números, así que estos casos se prueban contra el ejecutable directamente (son los que valida `test_program.py`):

| Caso | Entrada por stdin | Esperado | Exit |
|---|---|---|---|
| T27 nan | opción 1, a1=`nan` | `entrada no valida` | 1 |
| T28 inf | opción 1, a1=`inf` | `entrada no valida` | 1 |
| T29a opción no numérica | `a` | `entrada no valida` | 1 |
| T29b coeficiente no numérico | opción 1, a1=`a` | `entrada no valida` | 1 |
| T29c opción fuera de rango | `3` | `Opcion no valida` | 1 |

### 1.4 Verificaciones globales (las últimas 2 de los 37)

| Verificación | Qué comprueba |
|---|---|
| CSV | `trajectories.csv` tiene header `trajectory_id,t,x,y` y >1000 filas |
| PNG | `phase_portrait.png` se regenera en cada corrida válida |

Además, T07–T14, T21, T24, T25, T27–T29 verifican que el programa **no toca** el CSV ni el PNG (omisión intencional de la simulación cuando el punto crítico no es aislado o la entrada es inválida). Y T30 verifica que las órbitas del centro sean cerradas (`r_max/r_min < 1.5`).

---

## 2. Suite web — los 19 checks (5 escenarios)

Ejecutada por `tests/test_app_e2e.py` con Streamlit AppTest (sin servidor):

| # | Escenario | Checks |
|---|---|---|
| 1 | Carga inicial | sin excepción, mensaje inicial, botón Simular |
| 2 | Silla por defecto (1,0,0,-1) | tipo, estabilidad, gráfica, HTML autocontenido |
| 3 | No único (0,1,0,0) | mensaje NO ES UNICO, sin gráfica |
| 4 | Cambio a no lineal | resultado limpiado, 10 inputs nuevos |
| 5 | No lineal no hiperbólico (x'=y, y'=-x+x²) | Centro, advertencia Hartman-Grobman |

---

## 3. Notas

- Los coeficientes de las tablas siguen el orden (a1, a2, b1, b2) de los campos del sidebar; el orden por stdin del C++ es (a1, b1, a2, b2).
- T22 (1e-12) y T23 (1.0000000001) se pueden escribir tal cual en los campos de la web: el number_input acepta notación científica.
- La advertencia de Hartman-Grobman solo aparece en la web si el sistema es **no lineal** y el punto es **no hiperbólico**.
