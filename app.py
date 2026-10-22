import os
import re
import subprocess

import pandas as pd
import plotly.graph_objects as go
import plotly.io as pio
import streamlit as st

st.set_page_config(
    page_title="Sistemas Dinamicos 2D - Punto Critico (0,0)",
    layout="wide",
)

CSS = """
<style>
.block-container { padding-top: 2.2rem; max-width: 1120px; }
.sd-title { font-size: 1.55rem; font-weight: 650; color: #1a1a1a; margin-bottom: 0.1rem; }
.sd-subtitle { font-size: 0.92rem; color: #6b7280; margin-bottom: 1.4rem; }
.sd-card {
    border: 1px solid #e3e6ea;
    border-left: 4px solid #1f4e79;
    background: #fafbfc;
    border-radius: 4px;
    padding: 0.85rem 1.1rem;
    margin-bottom: 0.7rem;
}
.sd-card.sd-ok      { border-left-color: #1a7f37; }
.sd-card.sd-bad     { border-left-color: #b42318; }
.sd-card.sd-caution { border-left-color: #b45309; }
.sd-label {
    font-size: 0.72rem;
    text-transform: uppercase;
    letter-spacing: 0.07em;
    color: #6b7280;
    margin-bottom: 0.15rem;
}
.sd-value { font-size: 1.2rem; font-weight: 600; color: #1a1a1a; }
.sd-note {
    border: 1px solid #e3e6ea;
    border-left: 4px solid #b45309;
    background: #fffdf7;
    border-radius: 4px;
    padding: 0.7rem 1rem;
    font-size: 0.88rem;
    color: #5c4a1e;
    margin-bottom: 0.7rem;
}
.sd-hint { font-size: 0.88rem; color: #6b7280; }
.sd-empty {
    border: 1px dashed #ced4da;
    border-radius: 4px;
    padding: 1.6rem;
    text-align: center;
    color: #6b7280;
    font-size: 0.92rem;
}
</style>
"""
st.markdown(CSS, unsafe_allow_html=True)

TIPO_LABEL = {
    "Punto Silla": "Punto silla",
    "Nodo": "Nodo",
    "Nodo Estrella": "Nodo estrella",
    "Nodo Impropio (Degenerado)": "Nodo impropio (degenerado)",
    "Foco (Espiral)": "Foco (espiral)",
    "Centro": "Centro",
}
ESTABILIDAD_LABEL = {
    "Inestable": "Inestable",
    "Asintoticamente Estable": "Asintóticamente estable",
    "Estable (no asintotico)": "Estable (no asintótico)",
}
TIPO_COLOR = {
    "Punto silla": "#c77700",
    "Nodo": "#1f4e79",
    "Nodo estrella": "#1f4e79",
    "Nodo impropio (degenerado)": "#1f4e79",
    "Foco (espiral)": "#6a3d9a",
    "Centro": "#1a7f37",
}


def find_executable():
    candidates = [
        os.path.join("build", "SistemasDinamicos.exe"),
        os.path.join("build", "SistemasDinamicos"),
        "SistemasDinamicos.exe",
        "SistemasDinamicos",
    ]
    for path in candidates:
        if os.path.exists(path):
            return path
    return None


def parse_output(raw):
    result = {
        "raw": raw,
        "tipo": None,
        "estabilidad": None,
        "mensaje": None,
        "unique": True,
        "hg_no_aplica": False,
    }
    if "NO ES UNICO" in raw or "NO es un punto critico" in raw:
        result["unique"] = False
    m = re.search(r"Tipo de Punto Critico \(Linealizado\):\s*(.+)", raw)
    if m:
        result["tipo"] = m.group(1).strip()
    m = re.search(r"Estabilidad:\s*(.+)", raw)
    if m:
        result["estabilidad"] = m.group(1).strip()
    m = re.search(r"MENSAJE:\s*(.+)", raw)
    if m:
        result["mensaje"] = m.group(1).strip()
    result["hg_no_aplica"] = "NO (Punto No Hiperbolico)" in raw
    return result


def build_figure(df):
    t0 = df[df["t"] == df["t"].min()]
    rng = max(t0["x"].abs().max(), t0["y"].abs().max())
    margin = rng * 1.05 if rng > 0 else 1.0

    fig = go.Figure()
    for tid in df["trajectory_id"].unique():
        traj = df[df["trajectory_id"] == tid]
        xs, ys = traj["x"].values, traj["y"].values
        fig.add_trace(
            go.Scatter(
                x=xs,
                y=ys,
                mode="lines",
                line=dict(color="rgba(59,110,165,0.45)", width=1.2),
                hoverinfo="skip",
                showlegend=False,
            )
        )
        if len(xs) > 6:
            mid = len(xs) // 2
            fig.add_annotation(
                x=xs[mid],
                y=ys[mid],
                ax=xs[mid - 1],
                ay=ys[mid - 1],
                xref="x",
                yref="y",
                axref="x",
                ayref="y",
                showarrow=True,
                arrowhead=3,
                arrowsize=1.0,
                arrowwidth=1.4,
                arrowcolor="#1f4e79",
            )

    fig.add_hline(y=0, line_dash="dash", line_color="#495057", line_width=0.8)
    fig.add_vline(x=0, line_dash="dash", line_color="#495057", line_width=0.8)
    fig.add_trace(
        go.Scatter(
            x=[0],
            y=[0],
            mode="markers",
            marker=dict(color="#c1121f", size=9),
            name="Punto crítico (0,0)",
        )
    )

    fig.update_layout(
        title=dict(text="Retrato de fase alrededor del punto crítico (0,0)", font=dict(size=15)),
        xaxis=dict(
            title="x(t)",
            range=[-margin, margin],
            zeroline=False,
            showgrid=True,
            gridcolor="#e9ecef",
            constrain="domain",
        ),
        yaxis=dict(
            title="y(t)",
            range=[-margin, margin],
            zeroline=False,
            showgrid=True,
            gridcolor="#e9ecef",
            scaleanchor="x",
            scaleratio=1,
        ),
        plot_bgcolor="white",
        paper_bgcolor="white",
        font=dict(family="Arial, Helvetica, sans-serif", size=12, color="#212529"),
        margin=dict(l=48, r=24, t=52, b=44),
        legend=dict(orientation="h", y=1.03, x=1.0, xanchor="right"),
        hovermode="closest",
    )
    return fig


def build_html_report(result, fig, system_label):
    if result.get("unique"):
        tipo = TIPO_LABEL.get(result["tipo"], result["tipo"])
        est = ESTABILIDAD_LABEL.get(result["estabilidad"], result["estabilidad"])
        cards = (
            f'<div class="card"><div class="label">Tipo de punto crítico</div>'
            f'<div class="value">{tipo}</div></div>'
            f'<div class="card"><div class="label">Estabilidad</div>'
            f'<div class="value">{est}</div></div>'
        )
    else:
        mensaje = result.get("mensaje") or "EL PUNTO CRITICO (0,0) NO ES UNICO"
        cards = (
            f'<div class="card"><div class="label">Clasificación</div>'
            f'<div class="value">{mensaje}</div></div>'
        )
    warning = ""
    if result["hg_no_aplica"]:
        warning = (
            '<div class="note">Teorema de Hartman-Grobman no aplicable'
            " (punto no hiperbólico): la clasificación proviene de la"
            " linealización y los términos no lineales pueden alterar el"
            " comportamiento cualitativo.</div>"
        )
    fig_html = pio.to_html(
        fig, full_html=False, include_plotlyjs=True, config={"displaylogo": False}
    )
    return f"""<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="utf-8">
<title>Informe - Punto critico (0,0)</title>
<style>
body {{ font-family: Arial, Helvetica, sans-serif; color: #212529; max-width: 960px;
       margin: 2rem auto; padding: 0 1rem; }}
h1 {{ font-size: 1.35rem; font-weight: 650; margin-bottom: 0.2rem; }}
.sub {{ color: #6b7280; font-size: 0.9rem; margin-bottom: 1.4rem; }}
.card {{ border: 1px solid #e3e6ea; border-left: 4px solid #1f4e79;
        background: #fafbfc; border-radius: 4px; padding: 0.8rem 1.1rem;
        margin-bottom: 0.7rem; }}
.label {{ font-size: 0.72rem; text-transform: uppercase; letter-spacing: 0.07em;
         color: #6b7280; }}
.value {{ font-size: 1.15rem; font-weight: 600; }}
.note {{ border: 1px solid #e3e6ea; border-left: 4px solid #b45309;
        background: #fffdf7; border-radius: 4px; padding: 0.7rem 1rem;
        font-size: 0.88rem; color: #5c4a1e; margin-bottom: 0.7rem; }}
</style>
</head>
<body>
<h1>Análisis de sistemas dinámicos 2D</h1>
<div class="sub">Tipo y estabilidad del punto crítico (0,0) — {system_label}</div>
{cards}
{warning}
{fig_html}
</body>
</html>"""


def card(label, value, css_class=""):
    st.markdown(
        f'<div class="sd-card {css_class}"><div class="sd-label">{label}</div>'
        f'<div class="sd-value">{value}</div></div>',
        unsafe_allow_html=True,
    )


exe = find_executable()

st.markdown('<div class="sd-title">Análisis de sistemas dinámicos 2D</div>', unsafe_allow_html=True)
st.markdown(
    '<div class="sd-subtitle">Tipo y estabilidad del punto crítico (0,0) y retrato de fase</div>',
    unsafe_allow_html=True,
)

if exe is None:
    st.markdown(
        '<div class="sd-empty">No se encontró el ejecutable en build/. '
        "Compile el proyecto con <code>cmake --build build</code> y recargue esta página.</div>",
        unsafe_allow_html=True,
    )
    st.stop()

if "result" not in st.session_state:
    st.session_state.result = None

if "mode" not in st.session_state:
    st.session_state.mode = "Sistema lineal"


def on_mode_change():
    st.session_state.result = None


with st.sidebar:
    st.markdown("**Configuración del sistema**")
    mode = st.radio(
        "Tipo de sistema",
        ["Sistema lineal", "Sistema no lineal"],
        index=0 if st.session_state.mode == "Sistema lineal" else 1,
        key="mode",
        on_change=on_mode_change,
    )
    st.markdown("---")
    st.markdown("**Coeficientes**")

    with st.form(key="coefficients_form"):
        if mode == "Sistema lineal":
            c1, c2 = st.columns(2)
            with c1:
                a1 = st.number_input("a1(Coeficiente de x en  x')", value=1.0, step=1.0, key="l_a1")
                a2 = st.number_input("a2(Coeficiente de x en  y')", value=0.0, step=1.0, key="l_a2")
            with c2:
                b1 = st.number_input("b1(Coeficiente de y en  x')", value=0.0, step=1.0, key="l_b1")
                b2 = st.number_input("b2(Coeficiente de y en  y')", value=-1.0, step=1.0, key="l_b2")
            submitted = st.form_submit_button("Simular", width="stretch")
            input_data = f"1\n{a1}\n{b1}\n{a2}\n{b2}\n"
            system_label = f"Sistema lineal: a1={a1}, b1={b1}, a2={a2}, b2={b2}"
        else:
            eq1, eq2 = st.columns(2)
            with eq1:
                st.markdown("Ecuación x'")
                na1 = st.number_input("a1(Coeficiente de x en  x')", value=-1.0, step=1.0, key="n_a1")
                nb1 = st.number_input("b1(Coeficiente de y en  x')", value=-1.0, step=1.0, key="n_b1")
                nc1 = st.number_input("c1(Coeficiente de x en  y')", value=1.0, step=1.0, key="n_c1")
                nd1 = st.number_input("d1(Coeficiente de y en  x')", value=0.0, step=1.0, key="n_d1")
                ne1 = st.number_input("e1(Coeficiente de y en  y')", value=0.0, step=1.0, key="n_e1")
            with eq2:
                st.markdown("Ecuación y'")
                na2 = st.number_input("a2(Coeficiente de x en  y')", value=1.0, step=1.0, key="n_a2")
                nb2 = st.number_input("b2(Coeficiente de y en  y')", value=-1.0, step=1.0, key="n_b2")
                nc2 = st.number_input("c2(Coeficiente de x en  y')", value=0.0, step=1.0, key="n_c2")
                nd2 = st.number_input("d2(Coeficiente de y en  x')", value=0.0, step=1.0, key="n_d2")
                ne2 = st.number_input("e2(Coeficiente de y en  y')", value=0.0, step=1.0, key="n_e2")
            submitted = st.form_submit_button("Simular", width="stretch")
            input_data = (
                f"2\n{na1} {nb1} {nc1} {nd1} {ne1}\n"
                f"{na2} {nb2} {nc2} {nd2} {ne2}\n"
            )
            system_label = (
                f"Sistema no lineal: a1={na1}, b1={nb1}, c1={nc1}, d1={nd1}, e1={ne1}, "
                f"a2={na2}, b2={nb2}, c2={nc2}, d2={nd2}, e2={ne2}"
            )

    if submitted:
        try:
            proc = subprocess.run(
                [exe],
                input=input_data,
                capture_output=True,
                text=True,
                encoding="utf-8",
                errors="replace",
                cwd=os.getcwd(),
                timeout=120,
            )
            raw = proc.stdout + proc.stderr
            parsed = parse_output(raw)
            parsed["exit_code"] = proc.returncode
            parsed["system_label"] = system_label
            parsed["fig"] = None
            parsed["html"] = None
            if proc.returncode == 0:
                if os.path.exists("trajectories.csv"):
                    parsed["df"] = pd.read_csv("trajectories.csv")
                    parsed["fig"] = build_figure(parsed["df"])
                    parsed["html"] = build_html_report(parsed, parsed["fig"], system_label)
                else:
                    parsed["mensaje"] = "No se generó el archivo de trayectorias."
                    parsed["unique"] = False
            st.session_state.result = parsed
        except Exception as exc:
            st.session_state.result = {
                "raw": str(exc),
                "tipo": None,
                "estabilidad": None,
                "mensaje": f"Error al ejecutar el programa: {exc}",
                "unique": False,
                "hg_no_aplica": False,
                "exit_code": 1,
                "system_label": system_label,
                "fig": None,
                "html": None,
            }

result = st.session_state.result

st.markdown("### Informe")

if result is None:
    st.markdown(
        '<div class="sd-empty">Ingrese los coeficientes en la barra lateral'
        " y presione Simular para obtener el diagnóstico.</div>",
        unsafe_allow_html=True,
    )
else:
    if not result["unique"]:
        mensaje = result["mensaje"] or "EL PUNTO CRITICO (0,0) NO ES UNICO"
        card("Punto crítico", mensaje, css_class="sd-bad")
    else:
        tipo = TIPO_LABEL.get(result["tipo"], result["tipo"] or "-")
        estabilidad = ESTABILIDAD_LABEL.get(result["estabilidad"], result["estabilidad"] or "-")
        color = TIPO_COLOR.get(tipo, "#1f4e79")
        st.markdown(
            f'<div class="sd-card" style="border-left-color: {color};">'
            f'<div class="sd-label">Tipo de punto crítico</div>'
            f'<div class="sd-value">{tipo}</div></div>',
            unsafe_allow_html=True,
        )
        est_css = "sd-bad" if estabilidad == "Inestable" else (
            "sd-ok" if estabilidad == "Asintóticamente estable" else "sd-caution"
        )
        card("Estabilidad", estabilidad, css_class=est_css)
        if mode == "Sistema no lineal" and result["hg_no_aplica"]:
            st.markdown(
                '<div class="sd-note">Teorema de Hartman-Grobman no aplicable'
                " (punto no hiperbólico): la clasificación proviene de la"
                " linealización y los términos no lineales pueden alterar el"
                " comportamiento cualitativo.</div>",
                unsafe_allow_html=True,
            )

if result is not None and result.get("fig") is not None:
    st.markdown("### Retrato de fase")
    st.plotly_chart(
        result["fig"],
        width="stretch",
        config={"displaylogo": False},
    )
    if result.get("html"):
        st.download_button(
            label="Descargar informe HTML",
            data=result["html"],
            file_name="informe_punto_critico.html",
            mime="text/html",
        )
