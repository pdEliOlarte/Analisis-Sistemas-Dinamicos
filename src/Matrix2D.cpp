#include "Matrix2D.hpp" // Incluye la interfaz Matrix2D.hpp para implementar sus métodos miembros.
#include <cmath> // Incluye la librería de matemáticas para funciones como std::sqrt.

Matrix2D::Matrix2D(double a1_val, double b1_val, double a2_val, double b2_val) // Implementa el constructor parametrizado con los 4 coeficientes.
    : a1(a1_val), b1(b1_val), a2(a2_val), b2(b2_val) {} // Asigna cada valor recibido a su atributo correspondiente.

Matrix2D::Matrix2D() // Implementa el constructor por defecto sin parámetros.
    : a1(0.0), b1(0.0), a2(0.0), b2(0.0) {} // Inicializa todos los coeficientes con el valor 0.0.

double Matrix2D::getA1() const { // Implementación del método selector para recuperar a1.
    return a1; // Devuelve el valor numérico de a1.
} // Fin del método getA1.

double Matrix2D::getB1() const { // Implementación del método selector para recuperar b1.
    return b1; // Devuelve el valor numérico de b1.
} // Fin del método getB1.

double Matrix2D::getA2() const { // Implementación del método selector para recuperar a2.
    return a2; // Devuelve el valor numérico de a2.
} // Fin del método getA2.

double Matrix2D::getB2() const { // Implementación del método selector para recuperar b2.
    return b2; // Devuelve el valor numérico de b2.
} // Fin del método getB2.

double Matrix2D::computeTrace() const { // Implementación del método para calcular la traza de la matriz.
    return a1 + b2; // Suma los elementos de la diagonal principal para obtener Tr(A).
} // Fin del método computeTrace.

double Matrix2D::computeDeterminant() const { // Implementación del método para calcular el determinante de la matriz de 2x2.
    return (a1 * b2) - (a2 * b1); // Aplica la fórmula det(A) = a1*b2 - a2*b1.
} // Fin del método computeDeterminant.

double Matrix2D::computeDiscriminant() const { // Implementación del cálculo del discriminante característico del sistema.
    double trace = computeTrace(); // Obtiene la traza llamando al método computeTrace().
    double det = computeDeterminant(); // Obtiene el determinante llamando al método computeDeterminant().
    return (trace * trace) - (4.0 * det); // Aplica la ecuación Delta = Tr(A)^2 - 4*det(A).
} // Fin del método computeDiscriminant.

std::pair<std::complex<double>, std::complex<double>> Matrix2D::computeEigenvalues() const { // Implementación de la búsqueda de las raíces de la ecuación característica.
    double trace = computeTrace(); // Obtiene el valor de la traza Tr(A).
    double det = computeDeterminant(); // Obtiene el valor del determinante det(A).
    double disc = computeDiscriminant(); // Obtiene el valor del discriminante Delta.

    if (disc >= 0.0) { // Evalúa si las raíces de la ecuación característica son números reales (Delta >= 0).
        double root1 = (trace + std::sqrt(disc)) / 2.0; // Aplica la fórmula cuadrática con el signo positivo (+).
        double root2 = (trace - std::sqrt(disc)) / 2.0; // Aplica la fórmula cuadrática con el signo negativo (-).
        return {std::complex<double>(root1, 0.0), std::complex<double>(root2, 0.0)}; // Retorna los valores propios reales encapsulados en std::complex.
    } else { // Se ejecuta cuando el discriminante es estrictamente negativo (Delta < 0).
        double realPart = trace / 2.0; // Calcula la parte real común a ambos valores propios: Re(lambda) = Tr(A)/2.
        double imagPart = std::sqrt(-disc) / 2.0; // Calcula la magnitud de la parte imaginaria: Im(lambda) = sqrt(-Delta)/2.
        std::complex<double> lambda1(realPart, imagPart); // Asigna el primer valor propio complejo conjugado (alpha + i*beta).
        std::complex<double> lambda2(realPart, -imagPart); // Asigna el segundo valor propio complejo conjugado (alpha - i*beta).
        return {lambda1, lambda2}; // Retorna la pareja de valores propios complejos.
    } // Fin de la evaluación del discriminante.
} // Fin del método computeEigenvalues.