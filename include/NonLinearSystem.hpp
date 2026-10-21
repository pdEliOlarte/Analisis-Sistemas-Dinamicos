#ifndef NON_LINEAR_SYSTEM_HPP // Guard de inclusión para evitar definiciones múltiples de la cabecera.
#define NON_LINEAR_SYSTEM_HPP // Define la constante del guard de inclusión.

#include "Matrix2D.hpp" // Incluye Matrix2D para representar la matriz Jacobiana resultante.
#include <functional> // Incluye std::function para manejar ecuaciones funcionales f(x,y) y g(x,y).

// Clase que abstrae un sistema dinámico autónomo 2D no lineal x' = f(x,y), y' = g(x,y).
class NonLinearSystem {
private: // Funciones miembro privadas que definen el sistema.
    std::function<double(double, double)> f; // Almacena la función matemática de la primera ecuación x' = f(x,y).
    std::function<double(double, double)> g; // Almacena la función matemática de la segunda ecuación y' = g(x,y).

public: // Métodos públicos para evaluación numérica y linealización.
    NonLinearSystem(std::function<double(double, double)> f_func, std::function<double(double, double)> g_func); // Constructor que recibe las ecuaciones del sistema.

    double evalF(double x, double y) const; // Evalúa la velocidad dx/dt en el punto (x, y).
    double evalG(double x, double y) const; // Evalúa la velocidad dy/dt en el punto (x, y).

    bool isCriticalPoint(double x = 0.0, double y = 0.0, double tol = 1e-5) const; // Comprueba si F(x,y) = (0,0) en el punto dado.
    
    Matrix2D computeJacobian(double x = 0.0, double y = 0.0, double h = 1e-6) const; // Calcula numéricamente la matriz Jacobiana J(x,y) usando diferencias finitas.
}; // Fin de la declaración de la clase NonLinearSystem.

#endif // NON_LINEAR_SYSTEM_HPP // Fin de la protección del guard.