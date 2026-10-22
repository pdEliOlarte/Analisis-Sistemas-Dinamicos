# User Guide — Cómo usar el programa

Guía de uso del proyecto **Proyecto_Sistemas-Din-micos**. Hay dos formas de usarlo: la **interfaz web** (recomendada) y la **línea de comandos**.

---

## 1. Requisitos

- Windows (probado) con el proyecto completo.
- Python 3.14 del proyecto con dependencias ya instaladas en `.venv`:
  - `matplotlib`, `pandas` (gráficos del C++)
  - `streamlit`, `plotly` (interfaz web)
- Si falta algo: `.venv\Scripts\pip install matplotlib pandas streamlit plotly`
- Para recompilar el C++: `cmake` y `g++` (WinLibs, vía winget).

---

## 2. Interfaz web (recomendada)

### 2.1 Arrancar

```powershell
.venv\Scripts\streamlit.exe run app.py
```

Abre `http://localhost:8501` en el navegador (Streamlit lo abre solo). Para detener: Ctrl+C en la terminal, o cerrar la ventana.

### 2.2 Flujo de uso

1. **Barra lateral → Configuración del sistema**
   - `Sistema lineal` (por defecto): 4 campos — `a1`, `a2` a la izquierda; `b1`, `b2` a la derecha.
   - `Sistema no lineal`: 10 campos — `x'` (a1, b1, c1, d1, e1) e `y'` (a2, b2, c2, d2, e2). El sistema que modela es:
     ```
     x' = a1·x + b1·y + c1·x² + d1·y² + e1·x·y
     y' = a2·x + b2·y + c2·x² + d2·y² + e2·x·y
     ```
2. Pulsa **Simular**.
3. El informe central muestra:
   - **Tipo de punto crítico** (tarjeta con borde de color).
   - **Estabilidad** (verde = asintóticamente estable, rojo = inestable, ámbar = estable no asintótico).
   - Si el punto crítico **no es único**: mensaje `EL PUNTO CRITICO (0,0) NO ES UNICO` (la clasificación no se puede determinar) pero **igual se muestra el retrato de fase**.
   - Si es no lineal **no hiperbólico**: nota discreta de advertencia de Hartman-Grobman.
4. **Retrato de fase interactivo** (Plotly): zoom, hover, reset (icono de casa), descarga como PNG (icono de cámara). Ejemplo: los ejes, las flechas de flujo y el punto (0,0) en rojo.
5. **Descargar informe HTML**: genera `informe_punto_critico.html` autocontenido (se abre con doble clic, sin internet).

### 2.3 Tema

Claro por defecto (configurado en `.streamlit/config.toml`). Para cambiar: menú ⋮ de Streamlit → Settings → Appearance (también se puede fijar en `config.toml`).

### 2.4 Ejemplo rápido (silla)

- Modo lineal, valores: `a1=1`, `a2=0`, `b1=0`, `b2=-1`.
- Esperado: **Punto silla · Inestable** + retrato de fase con la clásica geometría de silla.

---

## 3. Línea de comandos (CLI)

### 3.1 Arrancar

```powershell
.\build\SistemasDinamicos.exe
```

### 3.2 Flujo

1. Elige opción:
   - `1` — Sistema lineal (pide `a1`, `b1`, `a2`, `b2` uno por línea).
   - `2` — Sistema no lineal (pide los 5 coeficientes de `x'` y luego los 5 de `y'`, separados por espacios).
2. El programa imprime el informe (tipo, estabilidad, valores propios, traza/determinante).
3. Genera `trajectories.csv` (RK4) y `phase_portrait.png` (matplotlib) también cuando el punto no es único (se grafica el comportamiento del campo aunque no se pueda clasificar).

### 3.3 Ejemplo

```text
Opcion (1 o 2): 1
  a1: 1
  b1: 0
  a2: 0
  b2: -1
```

Esperado: `Tipo de Punto Critico (Linealizado): Punto Silla`, `Estabilidad: Inestable`, y los archivos CSV + PNG.

### 3.4 Salidas del programa

| Mensaje | Significado |
|---|---|
| `Tipo de Punto Critico (Linealizado): X` | Clasificación (Nodo, Nodo Estrella, Nodo Impropio, Punto Silla, Foco (Espiral), Centro) |
| `Estabilidad: X` | Asintoticamente Estable / Estable (no asintotico) / Inestable |
| `MENSAJE: EL PUNTO CRITICO (0,0) NO ES UNICO` | `det = 0`: el origen no es punto crítico aislado; no se puede clasificar |
| `Teorema de Hartman-Grobman APLICA` | No lineal hiperbólico: la linealización es válida localmente |
| `NO (Punto No Hiperbolico)` | No lineal no hiperbólico: la clasificación es de la linealización y puede no ser exacta |
| `[AVISO] El origen no es un punto critico aislado; aun asi se grafica...` | El CSV y el PNG se generan igualmente (no es error) |
| `Error: entrada no valida...` | Entrada no numérica, `nan` o `inf`; exit code 1 |
| `Opcion no valida` | Opción fuera de {1, 2}; exit code 1 |

---

## 4. Compilar el C++ (solo si se modifica el código)

```powershell
cmake -S . -B build
cmake --build build
```

El exe queda en `build\SistemasDinamicos.exe`. Las DLL de MinGW están junto al exe; no hace falta tener el PATH de WinLibs configurado para ejecutarlo.

---

## 5. Ejecutar los tests

```powershell
# Suite C++ (caja negra): 37 checks
.venv\Scripts\python.exe tests\test_program.py

# Suite web (e2e con Streamlit AppTest): 20 checks
.venv\Scripts\python.exe tests\test_app_e2e.py
```

Los casos individuales (coeficientes y resultados esperados) están en `TESTPLAN.md`.

---

## 6. Solución de problemas

| Problema | Solución |
|---|---|
| El exe no arranca (código `0xC0000135`) | Faltan las DLL de MinGW junto al exe. Copiar `libstdc++-6.dll`, `libgcc_s_seh-1.dll`, `libwinpthread-1.dll` desde `mingw64\bin` de WinLibs a `build\`. |
| `Advertencia: fallo el script de graficos` | El `.venv` no tiene matplotlib/pandas: `.venv\Scripts\pip install matplotlib pandas`. |
| El PNG no aparece | Salida inválida (exit 1) o falló el script de matplotlib: revisar la consola del exe. Con coeficientes válidos siempre se genera, incluso si el punto no es único. |
| Puerto 8501 ocupado | Streamlit avisa y usa otro puerto (8502...), o detener el proceso previo. |
| La app no encuentra el exe | Compilar primero: `cmake --build build`. |
| El informe no muestra gráfica tras Simular | Pulsa **Simular** de nuevo (el informe se genera al enviar el formulario). |
