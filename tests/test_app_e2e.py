"""Tests e2e de la interfaz web (app.py) con Streamlit AppTest.

Uso:  .venv\\Scripts\\python.exe tests\\test_app_e2e.py

Ejecuta la app real de Streamlit, simula clics (formulario Simular, cambio
de modo) y verifica el informe, la grafica Plotly y el informe HTML descargable.
No requiere levantar el servidor: AppTest corre el script en un hilo aislado.
"""
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, ".venv", "Lib", "site-packages"))
os.chdir(ROOT)

from streamlit.testing.v1 import AppTest

APP = os.path.join(ROOT, "app.py")

fails = []


def check(cond, msg):
    if cond:
        print(f"[PASS] {msg}")
    else:
        print(f"[FAIL] {msg}")
        fails.append(msg)


def md_text(at):
    return "\n".join(str(m.value) for m in at.markdown)


def main():
    if not os.path.exists(APP):
        print(f"FALLO: no existe {APP}")
        sys.exit(2)

    # --- 1. carga inicial ---
    at = AppTest.from_file(APP, default_timeout=60)
    at.run()
    check(not at.exception, f"render inicial sin excepcion ({at.exception})")
    check("presione Simular" in md_text(at), "mensaje inicial presente")
    check(len(at.button) >= 1, f"boton Simular visible ({len(at.button)} botones)")

    # --- 2. silla por defecto (a1=1,b1=0,a2=0,b2=-1) ---
    at.button[0].click().run()
    check(not at.exception, f"Simular (silla) sin excepcion ({at.exception})")
    t = md_text(at)
    check("Punto silla" in t, "tipo = Punto silla")
    check("Inestable" in t, "estabilidad = Inestable")
    res = at.session_state["result"]
    check(res is not None and res.get("fig") is not None, "grafica generada")
    html = (res or {}).get("html") or ""
    check("<!DOCTYPE html>" in html, "informe HTML generado")
    check(len(html) > 100000, f"HTML autocontenido ({len(html)} bytes)")

    # --- 3. no unico (0,1,0,0) ---
    at.number_input(key="l_a1").set_value(0.0)
    at.number_input(key="l_b1").set_value(1.0)
    at.number_input(key="l_a2").set_value(0.0)
    at.number_input(key="l_b2").set_value(0.0)
    at.button[0].click().run()
    check(not at.exception, f"Simular (no unico) sin excepcion ({at.exception})")
    t = md_text(at)
    check("NO ES UNICO" in t, "mensaje NO ES UNICO en informe")
    res = at.session_state["result"]
    check(res is not None and res.get("unique") is False, "unique=False")
    check(res is not None and res.get("fig") is not None, "grafica presente aun si no unico")
    html = (res or {}).get("html") or ""
    check("<!DOCTYPE html>" in html and "NO ES UNICO" in (res or {}).get("raw", ""),
          "informe HTML generado para caso no unico")

    # --- 4. cambio a no lineal: limpia resultado, muestra inputs nuevos ---
    at.radio[0].set_value("Sistema no lineal").run()
    check(not at.exception, f"cambio a modo no lineal sin excepcion ({at.exception})")
    keys = [w.key for w in at.number_input]
    check("n_a1" in keys and "n_e2" in keys, f"inputs no lineales presentes ({len(keys)})")
    check(at.session_state["result"] is None, "resultado limpiado al cambiar modo")

    # --- 5. no lineal no hiperbolico: x'=y, y'=-x+x^2 (jacobiana = centro) ---
    at.number_input(key="n_a1").set_value(0.0)
    at.number_input(key="n_b1").set_value(1.0)
    at.number_input(key="n_c1").set_value(0.0)
    at.number_input(key="n_d1").set_value(0.0)
    at.number_input(key="n_e1").set_value(0.0)
    at.number_input(key="n_a2").set_value(-1.0)
    at.number_input(key="n_b2").set_value(0.0)
    at.number_input(key="n_c2").set_value(1.0)
    at.number_input(key="n_d2").set_value(0.0)
    at.number_input(key="n_e2").set_value(0.0)
    at.button[0].click().run()
    check(not at.exception, f"simulacion no lineal sin excepcion ({at.exception})")
    t = md_text(at)
    check("Centro" in t, "tipo = Centro")
    check("hiperb" in t.lower(), "advertencia Hartman-Grobman presente")

    print()
    if fails:
        print(f"RESULTADO: {len(fails)} fallaron")
        sys.exit(1)
    print("RESULTADO: TODO OK (5 escenarios e2e, 20 checks)")
    sys.exit(0)


if __name__ == "__main__":
    main()
