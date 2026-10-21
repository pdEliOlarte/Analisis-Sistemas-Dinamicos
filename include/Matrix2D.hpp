#ifndef MATRIX2D_HPP // Guard de inclusión para evitar redefiniciones al incluir la cabecera múltiples veces.
#define MATRIX2D_HPP // Define la constante única asociada a este archivo de encabezado.

#include <complex> // Incluye la librería estándar para el manejo de números complejos en valores propios.
#include <utility> // Incluye std::pair para retornar el par de valores propios (lambda1, lambda2).

// Definición de la clase Matrix2D para abstraer la matriz de coeficientes A de 2x2 del sistema lineal.
class Matrix2D {
private: // Declaración de atributos privados protegidos contra modificaciones externas.
    double a1; // Representa el coeficiente a1 de la primera ecuación lineal (x' = a1*x + b1*y).
    double b1; // Representa el coeficiente b1 de la primera ecuación lineal (x' = a1*x + b1*y).
    double a2; // Representa el coeficiente a2 de la segunda ecuación lineal (y' = a2*x + b2*y).
    double b2; // Representa el coeficiente b2 de la segunda ecuación lineal (y' = a2*x + b2*y).

public: // Métodos públicos para consulta, construcción y cálculo de propiedades algebraicas.
    Matrix2D(double a1_val, double b1_val, double a2_val, double b2_val); // Constructor parametrizado para inicializar la matriz.
    Matrix2D(); // Constructor por defecto que asigna cero a todos los coeficientes.

    double getA1() const; // Método getter para obtener el valor del coeficiente a1.
    double getB1() const; // Método getter para obtener el valor del coeficiente b1.
    double getA2() const; // Método getter para obtener el valor del coeficiente a2.
    double getB2() const; // Método getter para obtener el valor del coeficiente b2.

    double computeTrace() const; // Método para calcular la traza de la matriz: Tr(A) = a1 + b2.
    double computeDeterminant() const; // Método para calcular el determinante: det(A) = a1*b2 - a2*b1.
    double computeDiscriminant() const; // Método para calcular el discriminante: Delta = Tr(A)^2 - 4*det(A).

    std::pair<std::complex<double>, std::complex<double>> computeEigenvalues() const; // Método para calcular los dos valores propios (lambda1, lambda2).
}; // Fin de la declaración de la clase Matrix2D.

#endif // MATRIX2D_HPP // Fin de la protección contra inclusión múltiple del guard MATRIX2D_HPP.