# Contexto del proyecto

**Proyecto_Sistemas-Din-micos** — Tipo y estabilidad del punto crítico (0,0) en sistemas dinámicos 2D (lineales y no lineales), con retrato de fase geométrico.

## Objetivo

Programa de cómputo (tarea universitaria de sistemas dinámicos) que, dados los coeficientes de un sistema de EDOs 2D, responde:

1. **¿Es (0,0) un punto crítico único?** Si no lo es, lo informa pero igual grafica el retrato de fase.
2. **¿Qué tipo de punto crítico es?** Nodo, nodo estrella, nodo impropio, punto silla, foco (espiral) o centro.
3. **¿Es estable?** Asintóticamente estable, estable (no asintótico) o inestable.
4. **¿Cómo se ven las trayectorias?** Retrato de fase (campo de direcciones + órbitas) alrededor del origen.

## Qué problemas resuelve

- **Sistema lineal**: `x' = a1·x + b1·y`, `y' = a2·x + b2·y`. El punto crítico (0,0) es único sii `det(A) ≠ 0`. La clasificación sale de los valores propios de `A` (tabla de `Teoría.txt`).
- **Sistema no lineal cuadrático general**: `x' = a1·x + b1·y + c1·x² + d1·y² + e1·x·y`, `y' = a2·x + b2·y + c2·x² + d2·y² + e2·x·y`. Se evalúa el **Jacobiano en (0,0)** y se aplica el **teorema de Hartman-Grobman**: si el punto es hiperbólico (ningún valor propio con parte real nula), la linealización describe el comportamiento cualitativo local; si no lo es, se avisa explícitamente que la clasificación proviene de la linealización y puede no capturar la dinámica real.

## La máquina de clasificación

La lógica de decisión implementada en `SystemAnalyzer` replica exactamente la tabla de `AtenerEnCuenta/Teoría.txt`:

| Valores propios λ₁, λ₂ | Tipo |
|---|---|
| Reales, signos opuestos | Punto silla (inestable) |
| Reales, mismo signo (A ≠ λI) | Nodo (impropio si repetidos) |
| Reales, repetidos, A = λI | Nodo estrella |
| Complejos, Re(λ) ≠ 0 | Foco (espiral) |
| Puramente imaginarios | Centro (estable, no asintótico) |

| Condición | Estabilidad |
|---|---|
| Re(λ) < 0 para ambos | Asintóticamente estable |
| Re(λ) = 0 para ambos (centro) | Estable (no asintótico) |
| Re(λ) > 0 para alguno | Inestable |

`det(A) = 0` ⇒ **"EL PUNTO CRITICO (0,0) NO ES UNICO"** (el origen no es un punto crítico aislado); aun así se grafica el retrato de fase para mostrar el comportamiento del campo.

## Arquitectura

```
stdin (coeficientes)
   │
   ▼
main.cpp ──► Matrix2D (traza, det, valores propios)
   │        SystemAnalyzer (clasificación + H-G)
   │        NonLinearSystem (Jacobiano del sistema cuadrático)
   │        TrajectoryExporter (RK4 → trajectories.csv)
   ▼
scripts/plot_phase_portrait.py (matplotlib → phase_portrait.png)

app.py (Streamlit) ──► ejecuta build/SistemasDinamicos.exe como subprocess
                    ──► parsea la salida y muestra:
                        Tipo · Estabilidad · retrato de fase interactivo (Plotly)
                        + descarga de informe HTML autocontenido
```

- **Núcleo C++17** (`src/`, `include/`): todo el cálculo matemático y la simulación RK4.
- **Visualización Python** (`scripts/plot_phase_portrait.py`): retrato de fase PNG con matplotlib.
- **Interfaz web** (`app.py`): Streamlit + Plotly, todo local, sin dependencias externas. Ejecuta el mismo exe, no reimplementa la lógica.
- **Teoría y notas** (`AtenerEnCuenta/`): `Teoría.txt` (fuente de la clasificación), `Estructura.txt` (diseño de módulos), `Librerías.txt`, y estos documentos (`Contexto.md`, `User_Guide.md`).

## Decisiones clave

- **Epsilon de singularidad** `SINGULARITY_EPS = 1e-12` en la detección de `det ≈ 0`: evita clasificar como silla matrices numéricamente degeneradas (ver T22/T23 en `TESTPLAN.md`).
- **Validación de entrada**: `nan`/`inf`/no numéricos ⇒ `entrada no valida` + exit 1 (ver T27–T29).
- **Gráfica también si no único**: si el origen no es punto crítico aislado (`det = 0`), se mantiene el mensaje obligatorio `EL PUNTO CRITICO (0,0) NO ES UNICO` y se imprime `[AVISO] ... aun asi se grafica ...`; la simulación corre igual y se generan CSV y PNG (decisión del profesor: graficar de todas maneras los puntos que no pueden clasificarse).
- **Distinción estrella/impropio**: con valores propios repetidos, si `A = λI` es nodo estrella; si no, nodo impropio.
- **Ejes PNG fijos** (±rango simétrico) para que la gráfica no "respire" entre corridas.
- **DLLs de MinGW junto al exe**: el ejecutable corre en cualquier terminal o con doble clic, sin depender del PATH.
- **Todo local**: el informe HTML embebe Plotly.js (≈5 MB) para abrirse sin internet.

## Archivos de verificación

- `TESTPLAN.md`: los 37 checks de la suite C++ + los 20 de la suite web, formateados para la app.
- `tests/test_program.py`: suite de caja negra del exe (37 checks).
- `tests/test_app_e2e.py`: suite e2e de la interfaz web con Streamlit AppTest (20 checks).
