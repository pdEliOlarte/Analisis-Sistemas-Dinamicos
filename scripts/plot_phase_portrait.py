import os
import shutil
import sys
import matplotlib
import matplotlib.pyplot as plt
import pandas as pd

# Asigna el backend 'Agg' para renderizado en servidor/Codespaces
matplotlib.use('Agg')


def main():
    # Obtiene el nombre del archivo CSV enviado por el ejecutable de C++
    csv_file = sys.argv[1] if len(sys.argv) > 1 else "trajectories.csv"

    if not os.path.exists(csv_file):
        print(f"Error: El archivo {csv_file} no existe.")
        sys.exit(1)

    # Carga de la estructura de datos generada por la simulación
    df = pd.read_csv(csv_file)

    # Creación del lienzo principal
    fig, ax = plt.subplots(figsize=(8, 8))

    # Renderizado de trayectorias
    for traj_id in df["trajectory_id"].unique():
        traj_data = df[df["trajectory_id"] == traj_id]
        x_vals = traj_data["x"].values
        y_vals = traj_data["y"].values

        ax.plot(x_vals, y_vals, color="blue", alpha=0.35, linewidth=1.1)

        # Flecha indicadora del sentido del flujo temporal
        if len(x_vals) > 6:
            mid_idx = len(x_vals) // 2
            ax.annotate(
                "",
                xy=(x_vals[mid_idx], y_vals[mid_idx]),
                xytext=(x_vals[mid_idx - 1], y_vals[mid_idx - 1]),
                arrowprops=dict(arrowstyle="->", color="darkblue", lw=1.2)
            )

    # Ejes de coordenadas de referencia y punto crítico
    ax.axhline(0, color="black", linewidth=0.8, linestyle="--")
    ax.axvline(0, color="black", linewidth=0.8, linestyle="--")
    ax.plot(0, 0, "ro", markersize=8, label="Punto Critico (0,0)")

    # Formato visual del plano de fase
    ax.set_xlabel("Estado x(t)", fontsize=12)
    ax.set_ylabel("Estado y(t)", fontsize=12)
    ax.set_title("Retrato de Fase Alrededor del Punto Critico (0,0)", fontsize=14)
    ax.grid(True, linestyle=":", alpha=0.6)
    ax.legend(loc="upper right")
    ax.set_aspect("equal", adjustable="box")

    # Fija los ejes al rango de condiciones iniciales: si alguna trayectoria escapa
    # (sillas/nodos inestables), el autoescalado comprimiria la zona central.
    t0 = df[df["t"] == df["t"].min()]
    rng = max(t0["x"].abs().max(), t0["y"].abs().max())
    if rng > 0:
        margin = rng * 1.05
        ax.set_xlim(-margin, margin)
        ax.set_ylim(-margin, margin)

    # Guardar imagen en disco
    output_image = "phase_portrait.png"
    plt.savefig(output_image, dpi=300, bbox_inches="tight")
    plt.close(fig)
    print(f"\n[OK] Retrato de fase guardado en: {output_image}")

    # Abre automáticamente la imagen en el visor de pestañas de VS Code / Codespaces (solo si "code" existe)
    if shutil.which("code"):
        os.system(f"code -r {output_image}")


if __name__ == "__main__":
    main()