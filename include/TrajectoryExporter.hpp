#ifndef TRAJECTORY_EXPORTER_HPP // Guard de inclusión para evitar definiciones múltiples.
#define TRAJECTORY_EXPORTER_HPP // Define la constante única del encabezado.

#include "Matrix2D.hpp" // Incluye Matrix2D para exportar la simulación de sistemas lineales.
#include "NonLinearSystem.hpp" // Incluye NonLinearSystem para exportar la simulación de sistemas no lineales.
#include <string> // Incluye la librería de cadenas de texto para manipular rutas de archivos.

struct Point2D { // Estructura simple para representar una coordenada en el plano de fase.
    double x; // Estado x del sistema.
    double y; // Estado y del sistema.
}; // Fin de la estructura Point2D.

class TrajectoryExporter { // Clase responsable del cálculo numérico (RK4) y la escritura en CSV.
public: // Métodos públicos accesibles desde main.cpp.
    TrajectoryExporter() = default; // Constructor por defecto.

    // Método para exportar datos de simulación de un sistema lineal.
    bool exportSimulationData(const Matrix2D& matrix, const std::string& filename, double range = 5.0, double dt = 0.01, double tMax = 5.0, int gridDensity = 8) const;

    // NUEVO MÉTODO: Exporta los datos de simulación numérica de un sistema no lineal.
    bool exportSimulationDataNonLinear(const NonLinearSystem& system, const std::string& filename, double range = 5.0, double dt = 0.01, double tMax = 5.0, int gridDensity = 8) const;
}; // Fin de la declaración de la clase TrajectoryExporter.

#endif // TRAJECTORY_EXPORTER_HPP