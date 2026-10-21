#include "SystemAnalyzer.hpp" // Incluye la interfaz de SystemAnalyzer.
#include <iostream> // Incluye iostream para salida en consola.
#include <cmath> // Incluye cmath para operaciones matemáticas.

AnalysisResult SystemAnalyzer::analyze(const Matrix2D& matrix) const { // Método para análisis lineal de Matrix2D.
    AnalysisResult result; // Instancia la estructura del resultado.
    result.isHyperbolic = true; // Por defecto los lineales aislados con det != 0 son analizados directamente.
    result.hartmanGrobmanApplies = true; // Para lineales la clasificación es exacta.

    double det = matrix.computeDeterminant(); // Calcula det(A).
    if (std::abs(det) < SINGULARITY_EPS) { // Si det(A) ~= 0 (tolerancia 1e-12), el punto no es único.
        result.isUnique = false; // Marca unicidad como falsa.
        result.message = "EL PUNTO CRITICO (0,0) NO ES UNICO"; // Mensaje obligatorio.
        result.criticalPointType = "No determinado (recta/plano de equilibrios)"; // Clasificación.
        result.stability = "No aplica"; // Estabilidad.
        return result; // Retorna resultado no único.
    } // Fin condicional det = 0.

    result.isUnique = true; // Marca que el punto es único.
    result.message = "El punto critico (0,0) es unico."; // Mensaje de confirmación.

    auto eigenvalues = matrix.computeEigenvalues(); // Obtiene valores propios.
    std::complex<double> l1 = eigenvalues.first; // Extrae lambda1.
    std::complex<double> l2 = eigenvalues.second; // Extrae lambda2.

    bool isComplex = (std::abs(l1.imag()) > EPSILON); // Verifica si son complejos conjugados.

    if (isComplex) { // Clasificación para casos complejos.
        double alpha = l1.real(); // Parte real alpha.
        if (std::abs(alpha) < EPSILON) { // Si alpha = 0 (Valores imaginarios puros).
            result.isHyperbolic = false; // No es hiperbólico por tener Re(lambda) = 0.
            result.criticalPointType = "Centro"; // Clasifica como Centro.
            result.stability = "Estable (no asintotico)"; // Estabilidad.
        } else if (alpha > EPSILON) { // alpha > 0.
            result.criticalPointType = "Foco (Espiral)"; // Foco inestable.
            result.stability = "Inestable"; // Estabilidad inestable.
        } else { // alpha < 0.
            result.criticalPointType = "Foco (Espiral)"; // Foco estable.
            result.stability = "Asintoticamente Estable"; // Estabilidad asintótica.
        } // Fin condicional alpha.
    } else { // Clasificación para casos reales.
        double r1 = l1.real(); // Raíz real 1.
        double r2 = l2.real(); // Raíz real 2.

        // Signos con comparación EXACTA: det != 0 ya garantizado => lambda != 0.
        // (Con epsilon > 0 un lambda pequeñisimo, p.ej. 1e-12, caeria en "Degenerado".)
        if ((r1 > 0.0 && r2 < 0.0) || (r1 < 0.0 && r2 > 0.0)) { // Signos opuestos.
            result.criticalPointType = "Punto Silla"; // Clasifica como Punto Silla.
            result.stability = "Inestable"; // Inestable.
        } else if ((r1 > 0.0 && r2 > 0.0) || (r1 < 0.0 && r2 < 0.0)) { // Mismo signo => Nodo.
            double disc = matrix.computeDiscriminant(); // Delta = tau^2 - 4*det.
            if (std::abs(disc) < SINGULARITY_EPS) { // Delta = 0 => autovalor repetido (D = 0).
                // Distingue estrella (A = lambda*I, cualquier direccion es eigenvector) de impropio.
                double lambda = r1; // Autovalor doble (r1 = r2 = tau/2).
                double tol = 1e-9 * (std::abs(lambda) > 1.0 ? std::abs(lambda) : 1.0); // Tolerancia escalada al tamano de lambda.
                bool isScalarMatrix = std::abs(matrix.getA1() - lambda) < tol && // A11 = lambda.
                                      std::abs(matrix.getB2() - lambda) < tol && // A22 = lambda.
                                      std::abs(matrix.getB1()) < tol &&          // A12 = 0.
                                      std::abs(matrix.getA2()) < tol;            // A21 = 0.
                result.criticalPointType = isScalarMatrix ? "Nodo Estrella" : "Nodo Impropio (Degenerado)"; // Tipo D=0.
            } else { // Delta != 0 => nodo clasico (dos direcciones propias distintas).
                result.criticalPointType = "Nodo"; // Nodo comun.
            } // Fin condicional Delta = 0.
            result.stability = (r1 > 0.0) ? "Inestable" : "Asintoticamente Estable"; // Estabilidad por signo del autovalor.
        } else { // Caso limite con raíz nula (practicamente inalcanzable tras el corte por det).
            result.isHyperbolic = false; // No es hiperbólico por raíz nula.
            result.criticalPointType = "Degenerado"; // Punto degenerado.
            result.stability = "Indeterminado"; // Indeterminado.
        } // Fin condicional raíces reales.
    } // Fin evaluación de valores propios.

    result.hartmanGrobmanApplies = result.isHyperbolic; // La aplicabilidad exige hiperbolicidad: evita decir "SI (Punto Hiperbolico)" cuando Re(lambda)=0.

    return result; // Devuelve el resultado.
} // Fin del método analyze lineal.

AnalysisResult SystemAnalyzer::analyzeNonLinear(const NonLinearSystem& system, double x0, double y0) const { // Implementación para sistemas no lineales.
    AnalysisResult result; // Instancia la estructura del resultado.

    if (!system.isCriticalPoint(x0, y0)) { // Verifica si F(x0, y0) = (0,0).
        result.isUnique = false; // Indica que el punto no es de equilibrio.
        result.message = "El punto ingresado NO es un punto critico del sistema no lineal."; // Mensaje de advertencia.
        result.criticalPointType = "No es un punto critico"; // Tipo no válido.
        result.stability = "No aplica"; // Estabilidad no aplicable.
        return result; // Retorna temprano.
    } // Fin comprobación de punto crítico.

    Matrix2D jacobian = system.computeJacobian(x0, y0); // Calcula la matriz Jacobiana J(x0, y0).
    result = analyze(jacobian); // Analiza la matriz Jacobiana usando el clasificador lineal.

    if (!result.isHyperbolic) { // Aplica la condición del Teorema de Hartman-Grobman sobre hiperbolicidad.
        result.hartmanGrobmanApplies = false; // Registra que Hartman-Grobman no aplica.
        result.message += " [ADVERTENCIA: El punto es NO HIPERBOLICO. El Teorema de Hartman-Grobman NO APLICA; los terminos no lineales determinan el comportamiento cualitativo real.]"; // Añade advertencia teórica.
    } else { // Si el punto es hiperbólico (Re(lambda) != 0).
        result.hartmanGrobmanApplies = true; // Confirma la validez del teorema de Hartman-Grobman.
        result.message += " [Teorema de Hartman-Grobman APLICA: El sistema no lineal es cualitativamente equivalente a su linealizacion.]"; // Añade confirmación.
    } // Fin evaluación de Hartman-Grobman.

    return result; // Retorna el diagnóstico no lineal completado.
} // Fin de analyzeNonLinear.

void SystemAnalyzer::printReport(const AnalysisResult& result) const { // Imprime el reporte cualitativo.
    std::cout << "========================================\n"; // Línea divisoria.
    std::cout << "     DIAGNOSTICO DEL SISTEMA DINAMICO    \n"; // Título del reporte.
    std::cout << "========================================\n"; // Línea divisoria.
    
    if (!result.isUnique) { // Si el punto no es único o no es crítico.
        std::cout << "MENSAJE: " << result.message << "\n"; // Muestra el mensaje de advertencia.
    } else { // Si el punto es único y crítico.
        std::cout << "Unicidad: " << result.message << "\n"; // Muestra la confirmación de unicidad.
        std::cout << "Tipo de Punto Critico (Linealizado): " << result.criticalPointType << "\n"; // Tipo de punto crítico.
        std::cout << "Estabilidad: " << result.stability << "\n"; // Nivel de estabilidad.
        std::cout << "Aplica Hartman-Grobman: " << (result.hartmanGrobmanApplies ? "SI (Punto Hiperbolico)" : "NO (Punto No Hiperbolico)") << "\n"; // Estado de Hartman-Grobman.
    } // Fin condicional de impresión.
    
    std::cout << "========================================\n"; // Línea divisoria final.
} // Fin de printReport.