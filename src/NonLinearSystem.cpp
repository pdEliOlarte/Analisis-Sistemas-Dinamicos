#include "NonLinearSystem.hpp" // Incluye la interfaz de la clase NonLinearSystem.
#include <cmath> // Incluye std::abs para validación de tolerancias numéricas.

NonLinearSystem::NonLinearSystem(std::function<double(double, double)> f_func, std::function<double(double, double)> g_func) // Implementación del constructor.
    : f(f_func), g(g_func) {} // Inicializa los objetos std::function con las expresiones del sistema.

double NonLinearSystem::evalF(double x, double y) const { // Implementación de la evaluación de x'.
    return f(x, y); // Devuelve el resultado de evaluar f(x,y).
} // Fin del método evalF.

double NonLinearSystem::evalG(double x, double y) const { // Implementación de la evaluación de y'.
    return g(x, y); // Devuelve el resultado de evaluar g(x,y).
} // Fin del método evalG.

bool NonLinearSystem::isCriticalPoint(double x, double y, double tol) const { // Comprueba la condición F(x,y) = (0,0).
    return (std::abs(f(x, y)) < tol) && (std::abs(g(x, y)) < tol); // Retorna true si ambas componentes son nulas dentro de la tolerancia.
} // Fin del método isCriticalPoint.

Matrix2D NonLinearSystem::computeJacobian(double x, double y, double h) const { // Calcula la matriz Jacobiana en (x,y) mediante derivadas numéricas.
    double df_dx = (f(x + h, y) - f(x - h, y)) / (2.0 * h); // Calcula la derivada parcial df/dx usando diferencias finitas centradas.
    double df_dy = (f(x, y + h) - f(x, y - h)) / (2.0 * h); // Calcula la derivada parcial df/dy usando diferencias finitas centradas.
    double dg_dx = (g(x + h, y) - g(x - h, y)) / (2.0 * h); // Calcula la derivada parcial dg/dx usando diferencias finitas centradas.
    double dg_dy = (g(x, y + h) - g(x, y - h)) / (2.0 * h); // Calcula la derivada parcial dg/dy usando diferencias finitas centradas.

    return Matrix2D(df_dx, df_dy, dg_dx, dg_dy); // Construye y devuelve la matriz Jacobiana J como un objeto Matrix2D.
} // Fin del método computeJacobian.