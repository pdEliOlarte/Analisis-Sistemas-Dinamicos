"""Tests de caja negra para SistemasDinamicos.exe.

Uso:  .venv\\Scripts\\python.exe tests\\test_program.py

Cada caso define: entrada por stdin + substrings que DEBEN aparecer en la salida.
- skip_files=True: el programa NO debe tocar trajectories.csv ni phase_portrait.png
  (casos de punto no unico y entradas invalidas: se omite la simulacion a proposito).
- exit_code: codigo de salida esperado del proceso.
- orbit=True: verifica ademas que las orbitas del centro sean cerradas (r_max/r_min < 1.5).
"""
import csv
import math
import os
import subprocess
import sys
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "build", "SistemasDinamicos.exe")
CSV = os.path.join(ROOT, "trajectories.csv")
PNG = os.path.join(ROOT, "phase_portrait.png")

NO_UNICO = ["EL PUNTO CRITICO (0,0) NO ES UNICO", "Se omite la simulacion"]
INVALIDA = ["entrada no valida"]


def case(name, stdin, expect, skip_files=False, exit_code=0, orbit=False):
    return {
        "name": name,
        "stdin": stdin,
        "expect": expect,
        "skip_files": skip_files,
        "exit_code": exit_code,
        "orbit": orbit,
    }


TESTS = [
    # ===== Casos de la tabla (T01 - T30) =====
    case("T01 (1,0,0,1) D=0 estrella", "1\n1\n0\n0\n1\n",
         ["unico", "Nodo", "Estabilidad: Inestable"]),
    case("T02 (-1,0,0,-1) D=0 estable", "1\n-1\n0\n0\n-1\n",
         ["unico", "Nodo", "Asintoticamente Estable"]),
    case("T03 (0,1,-1,0) Centro", "1\n0\n1\n-1\n0\n",
         ["Centro", "Estable (no asintotico)", "NO (Punto No Hiperbolico)"]),
    case("T04 (0,-1,1,0) Centro orientacion opuesta", "1\n0\n-1\n1\n0\n",
         ["Centro", "Estable (no asintotico)"]),
    case("T05 (1,0,0,-1) Silla", "1\n1\n0\n0\n-1\n",
         ["Punto Silla", "Estabilidad: Inestable"]),
    case("T06 (0,1,1,0) Silla", "1\n0\n1\n1\n0\n",
         ["Punto Silla", "Estabilidad: Inestable"]),
    case("T07 (0,1,0,0) det=0", "1\n0\n1\n0\n0\n", NO_UNICO, skip_files=True),
    case("T08 (1,1,1,1) filas proporcionales", "1\n1\n1\n1\n1\n", NO_UNICO, skip_files=True),
    case("T09 (0,0,0,0) matriz nula", "1\n0\n0\n0\n0\n", NO_UNICO, skip_files=True),
    case("T10 (1,0,0,0) det=0", "1\n1\n0\n0\n0\n", NO_UNICO, skip_files=True),
    case("T11 (0,0,1,1) det=0", "1\n0\n0\n1\n1\n", NO_UNICO, skip_files=True),
    case("T12 (1,1,0,0) det=0", "1\n1\n1\n0\n0\n", NO_UNICO, skip_files=True),
    case("T13 (1,2,2,4) det=0 exacto", "1\n1\n2\n2\n4\n", NO_UNICO, skip_files=True),
    case("T14 (1,2,3,6) filas prop. distintas", "1\n1\n2\n3\n6\n", NO_UNICO, skip_files=True),
    case("T15 (1,0,0,2) nodo inestable", "1\n1\n0\n0\n2\n",
         ["Nodo", "Estabilidad: Inestable"]),
    case("T16 (-1,0,0,-2) nodo estable", "1\n-1\n0\n0\n-2\n",
         ["Nodo", "Asintoticamente Estable"]),
    case("T17 (1,1,-1,1) foco inestable", "1\n1\n1\n-1\n1\n",
         ["Foco (Espiral)", "Estabilidad: Inestable"]),
    case("T18 (-1,1,-1,-1) foco estable", "1\n-1\n1\n-1\n-1\n",
         ["Foco (Espiral)", "Asintoticamente Estable"]),
    case("T19 (2,1,0,2) D=0 impropio", "1\n2\n1\n0\n2\n",
         ["Nodo", "Estabilidad: Inestable"]),
    case("T20 (-2,1,0,-2) D=0 impropio estable", "1\n-2\n1\n0\n-2\n",
         ["Nodo", "Asintoticamente Estable"]),
    case("T21 (0,0,1,0) det=0", "1\n0\n0\n1\n0\n", NO_UNICO, skip_files=True),
    case("T22 (1,0,0,1e-12) casi degenerado", "1\n1\n0\n0\n1e-12\n",
         ["unico", "Nodo", "Estabilidad: Inestable"]),
    case("T23 (1,1,1,1.0000000001) det~1e-10", "1\n1\n1\n1\n1.0000000001\n",
         ["unico", "Nodo", "Estabilidad: Inestable"]),
    case("T24 (0.1,0.2,0.3,0.6) det=0 en FP", "1\n0.1\n0.2\n0.3\n0.6\n",
         NO_UNICO, skip_files=True),
    case("T25 (1e6,1e6,1e6,1e6) sin overflow", "1\n1e6\n1e6\n1e6\n1e6\n",
         NO_UNICO, skip_files=True),
    case("T26 (2^31-1,1,1,2^31-1) sin overflow", "1\n2147483647\n1\n1\n2147483647\n",
         ["unico", "Nodo", "Estabilidad: Inestable"]),
    case("T27 (nan) entrada invalida", "1\nnan\n0\n0\n1\n",
         INVALIDA, skip_files=True, exit_code=1),
    case("T28 (inf) entrada invalida", "1\ninf\n0\n0\n1\n",
         INVALIDA, skip_files=True, exit_code=1),
    case("T29a opcion no numerica", "a\n", INVALIDA, skip_files=True, exit_code=1),
    case("T29b coeficiente no numerico", "1\na\n0\n0\n1\n",
         INVALIDA, skip_files=True, exit_code=1),
    case("T29c opcion fuera de rango", "3\n",
         ["Opcion no valida"], skip_files=True, exit_code=1),
    case("T30 (0,1,-1,0) Centro con grafica", "1\n0\n1\n-1\n0\n",
         ["Centro", "Estable (no asintotico)"], orbit=True),

    # ===== Casos adicionales de foco y no lineal =====
    case("Foco estable (-0.5,-1,1,-0.5)", "1\n-0.5\n-1\n1\n-0.5\n",
         ["Foco (Espiral)", "Asintoticamente Estable"]),
    case("Foco inestable (0.5,-1,1,0.5)", "1\n0.5\n-1\n1\n0.5\n",
         ["Foco (Espiral)", "Estabilidad: Inestable"]),
    case("No lineal: foco estable + Hartman-Grobman", "2\n-1 -1 1 0 0\n1 -1 0 0 0\n",
         ["Foco (Espiral)", "Asintoticamente Estable", "Teorema de Hartman-Grobman APLICA"]),
]


def mtime(path):
    return os.path.getmtime(path) if os.path.exists(path) else 0.0


def run_exe(stdin_data, timeout=120):
    return subprocess.run(
        [EXE],
        input=stdin_data,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        cwd=ROOT,
        timeout=timeout,
    )


def check_center_orbits():
    """Verifica que las trayectorias del centro mantengan radio casi constante."""
    trajs = {}
    with open(CSV, newline="", encoding="utf-8") as f:
        for row in csv.DictReader(f):
            trajs.setdefault(int(row["trajectory_id"]), []).append(
                (float(row["x"]), float(row["y"]))
            )
    checked = bad = 0
    for pts in trajs.values():
        rs = [math.hypot(x, y) for x, y in pts]
        if rs[0] < 0.5:
            continue
        checked += 1
        if max(rs) / min(rs) > 1.5:
            bad += 1
    return checked, bad


def main():
    if not os.path.exists(EXE):
        print(f"FALLO: no existe {EXE} - compila primero con cmake --build build")
        sys.exit(2)

    passed = failed = 0
    failures = []

    for t in TESTS:
        before = (mtime(CSV), mtime(PNG))
        proc = run_exe(t["stdin"])
        out = proc.stdout + proc.stderr

        problems = [f"falta en salida: {e!r}" for e in t["expect"] if e not in out]
        if t["exit_code"] is not None and proc.returncode != t["exit_code"]:
            problems.append(f"exit={proc.returncode}, esperado {t['exit_code']}")
        if t["skip_files"]:
            after = (mtime(CSV), mtime(PNG))
            if after != before:
                problems.append("CSV/PNG fueron modificados (deberia omitirlos)")
        if t["orbit"] and not problems:
            checked, bad = check_center_orbits()
            if checked < 20:
                problems.append(f"solo {checked} orbitas verificables")
            elif bad > 0:
                problems.append(f"{bad} orbitas abiertas (r_max/r_min > 1.5)")

        if problems:
            failed += 1
            failures.append((t["name"], problems))
            print(f"[FAIL] {t['name']}")
            for p in problems:
                print(f"       {p}")
        else:
            passed += 1
            print(f"[PASS] {t['name']}")

    # Verificacion global: CSV generado con formato correcto
    if os.path.exists(CSV):
        with open(CSV, encoding="utf-8", errors="replace") as f:
            header = f.readline().strip()
            rows = sum(1 for _ in f)
        if header == "trajectory_id,t,x,y" and rows > 1000:
            passed += 1
            print(f"[PASS] CSV trajectories.csv (header OK, {rows} filas)")
        else:
            failed += 1
            print(f"[FAIL] CSV malformado: header={header!r} filas={rows}")
            failures.append(("CSV", ["header correcto y >1000 filas"]))
    else:
        failed += 1
        print("[FAIL] No se genero trajectories.csv")
        failures.append(("CSV", ["archivo existe"]))

    # Verificacion global: el retrato de fase debe (re)generarse en corridas validas
    png_before = mtime(PNG)
    run_exe("1\n1\n0\n0\n-1\n")
    time.sleep(0.5)
    if mtime(PNG) > png_before and mtime(PNG) > time.time() - 60:
        passed += 1
        print("[PASS] phase_portrait.png se regenera en cada corrida valida")
    else:
        failed += 1
        print("[FAIL] phase_portrait.png NO se regenera")
        failures.append(("Grafico", ["png regenerado"]))

    print("\n" + "=" * 60)
    print(f"RESULTADO: {passed} pasaron, {failed} fallaron de {passed + failed}")
    if failures:
        print("FALLOS:")
        for name, problems in failures:
            print(f"  - {name}: {problems}")
    print("=" * 60)
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
