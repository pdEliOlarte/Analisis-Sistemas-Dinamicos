#include "TrajectoryExporter.hpp" // Incluye la interfaz de la clase TrajectoryExporter.
#include <fstream> // Incluye la librería de flujos de archivos para crear y escribir el CSV.
#include <cmath> // Incluye la librería de matemáticas para funciones como std::abs.

bool TrajectoryExporter::exportSimulationData(const Matrix2D& matrix, const std::string& filename, double range, double dt, double tMax, int gridDensity) const { // Implementación para sistemas lineales.
    std::ofstream outFile(filename); // Abre el archivo CSV en disco para escritura.
    if (!outFile.is_open()) return false; // Retorna false si no se pudo crear el archivo.

    outFile << "trajectory_id,t,x,y\n"; // Escribe el encabezado de las columnas del CSV.
    int trajectoryId = 0; // Inicializa el identificador numérico de las trayectorias.
    double stepSize = (2.0 * range) / gridDensity; // Calcula la separación entre las condiciones iniciales.

    for (int i = 0; i <= gridDensity; ++i) { // Recorre los puntos iniciales sobre el eje x.
        double x0 = -range + i * stepSize; // Asigna el valor inicial de x0.
        for (int j = 0; j <= gridDensity; ++j) { // Recorre los puntos iniciales sobre el eje y.
            double y0 = -range + j * stepSize; // Asigna el valor inicial de y0.

            if (std::abs(x0) < 1e-5 && std::abs(y0) < 1e-5) continue; // Evita integrar directamente en el origen (0,0).

            Point2D current = {x0, y0}; // Inicializa la condición inicial actual.
            double t = 0.0; // Fija el tiempo inicial t = 0.
            int numSteps = static_cast<int>(tMax / dt); // Calcula la cantidad de pasos de integración temporal.

            for (int step = 0; step <= numSteps; ++step) { // Bucle de integración RK4.
                outFile << trajectoryId << "," << t << "," << current.x << "," << current.y << "\n"; // Escribe el punto actual en el CSV.

                if (std::abs(current.x) > range * 3.0 || std::abs(current.y) > range * 3.0) break; // Detiene si la trayectoria diverge.

                // Coeficientes del método Runge-Kutta de orden 4 (RK4) para el sistema lineal x' = Ax
                Point2D k1 = { matrix.getA1()*current.x + matrix.getB1()*current.y, matrix.getA2()*current.x + matrix.getB2()*current.y };
                Point2D k2 = { matrix.getA1()*(current.x + 0.5*dt*k1.x) + matrix.getB1()*(current.y + 0.5*dt*k1.y), matrix.getA2()*(current.x + 0.5*dt*k1.x) + matrix.getB2()*(current.y + 0.5*dt*k1.y) };
                Point2D k3 = { matrix.getA1()*(current.x + 0.5*dt*k2.x) + matrix.getB1()*(current.y + 0.5*dt*k2.y), matrix.getA2()*(current.x + 0.5*dt*k2.x) + matrix.getB2()*(current.y + 0.5*dt*k2.y) };
                Point2D k4 = { matrix.getA1()*(current.x + dt*k3.x) + matrix.getB1()*(current.y + dt*k3.y), matrix.getA2()*(current.x + dt*k3.x) + matrix.getB2()*(current.y + dt*k3.y) };

                current.x += (dt / 6.0) * (k1.x + 2.0 * k2.x + 2.0 * k3.x + k4.x); // Actualiza la posición x.
                current.y += (dt / 6.0) * (k1.y + 2.0 * k2.y + 2.0 * k3.y + k4.y); // Actualiza la posición y.
                t += dt; // Incrementa el paso de tiempo.
            } // Fin de la integración RK4.
            trajectoryId++; // Incrementa el ID de la trayectoria.
        } // Fin del bucle en y.
    } // Fin del bucle en x.

    outFile.close(); // Cierra el archivo CSV.
    return true; // Reporta éxito en la operación.
} // Fin del método exportSimulationData lineal.

bool TrajectoryExporter::exportSimulationDataNonLinear(const NonLinearSystem& system, const std::string& filename, double range, double dt, double tMax, int gridDensity) const { // Implementación para sistemas no lineales.
    std::ofstream outFile(filename); // Abre el archivo CSV en disco para escritura.
    if (!outFile.is_open()) return false; // Retorna false si no se pudo crear el archivo.

    outFile << "trajectory_id,t,x,y\n"; // Escribe la cabecera de las columnas del CSV.
    int trajectoryId = 0; // Inicializa el identificador numérico de las trayectorias.
    double stepSize = (2.0 * range) / gridDensity; // Calcula la separación entre las condiciones iniciales.

    for (int i = 0; i <= gridDensity; ++i) { // Recorre los puntos iniciales sobre el eje x.
        double x0 = -range + i * stepSize; // Asigna el valor inicial de x0.
        for (int j = 0; j <= gridDensity; ++j) { // Recorre los puntos iniciales sobre el eje y.
            double y0 = -range + j * stepSize; // Asigna el valor inicial de y0.

            if (std::abs(x0) < 1e-5 && std::abs(y0) < 1e-5) continue; // Evita integrar directamente en el origen (0,0).

            Point2D current = {x0, y0}; // Inicializa la condición inicial actual.
            double t = 0.0; // Fija el tiempo inicial t = 0.
            int numSteps = static_cast<int>(tMax / dt); // Calcula la cantidad de pasos de integración temporal.

            for (int step = 0; step <= numSteps; ++step) { // Bucle de integración RK4.
                outFile << trajectoryId << "," << t << "," << current.x << "," << current.y << "\n"; // Registra la posición en el CSV.

                if (std::abs(current.x) > range * 3.0 || std::abs(current.y) > range * 3.0) break; // Detiene si la trayectoria se aleja demasiado.

                // Coeficientes del método Runge-Kutta de orden 4 (RK4) para el sistema no lineal x' = f(x,y), y' = g(x,y)
                Point2D k1 = { system.evalF(current.x, current.y), system.evalG(current.x, current.y) };
                Point2D k2 = { system.evalF(current.x + 0.5*dt*k1.x, current.y + 0.5*dt*k1.y), system.evalG(current.x + 0.5*dt*k1.x, current.y + 0.5*dt*k1.y) };
                Point2D k3 = { system.evalF(current.x + 0.5*dt*k2.x, current.y + 0.5*dt*k2.y), system.evalG(current.x + 0.5*dt*k2.x, current.y + 0.5*dt*k2.y) };
                Point2D k4 = { system.evalF(current.x + dt*k3.x, current.y + dt*k3.y), system.evalG(current.x + dt*k3.x, current.y + dt*k3.y) };

                current.x += (dt / 6.0) * (k1.x + 2.0 * k2.x + 2.0 * k3.x + k4.x); // Actualiza la posición x.
                current.y += (dt / 6.0) * (k1.y + 2.0 * k2.y + 2.0 * k3.y + k4.y); // Actualiza la posición y.
                t += dt; // Incrementa el paso de tiempo.
            } // Fin de la integración RK4 no lineal.
            trajectoryId++; // Incrementa el ID de la trayectoria.
        } // Fin del bucle en y.
    } // Fin del bucle en x.

    outFile.close(); // Cierra el archivo CSV.
    return true; // Reporta éxito en la operación.
} // Fin del método exportSimulationDataNonLinear.