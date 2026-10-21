#include "Matrix2D.hpp" // Incluye la clase Matrix2D para el análisis de la matriz de coeficientes.
#include "NonLinearSystem.hpp" // Incluye la clase NonLinearSystem para abstraer el sistema no lineal y su Jacobiano.
#include "SystemAnalyzer.hpp" // Incluye SystemAnalyzer para el diagnóstico cualitativo y aplicación del Teorema de Hartman-Grobman.
#include "TrajectoryExporter.hpp" // Incluye TrajectoryExporter para la simulación numérica RK4 y exportación a CSV.
#include <iostream> // Incluye la librería estándar para la interacción mediante la consola.
#include <cstdlib> // Incluye cstdlib para habilitar llamadas a comandos del sistema mediante std::system.
#include <string> // Incluye la librería de cadenas de texto para manipular rutas de archivos e instrucciones.
#include <filesystem> // Incluye filesystem para detectar el intérprete de Python (venv) antes de invocar el script de gráficos.
#include <cmath> // Incluye cmath para std::isfinite: rechaza coeficientes NaN o infinitos.

// Busca un intérprete de Python con las dependencias instaladas (prefiere el venv del proyecto).
static std::string findPython() {
    // Rutas relativas sin espacios ni comillas: cmd.exe falla si el comando inicia con comillas.
    const char* candidates[] = {
        ".venv\\Scripts\\python.exe", "venv\\Scripts\\python.exe",       // venv de Windows.
        "..\\.venv\\Scripts\\python.exe", "..\\venv\\Scripts\\python.exe", // venv si el exe corre desde build/.
        ".venv/bin/python", "venv/bin/python",                           // venv de Linux/macOS.
        "../.venv/bin/python", "../venv/bin/python"
    };
    for (const char* candidate : candidates) {
        if (std::filesystem::exists(candidate)) return candidate;
    }
#ifdef _WIN32
    return "python"; // Respaldo en Windows: el alias "python3" de la Microsoft Store suele faltar dependencias.
#else
    return "python3"; // Respaldo en sistemas POSIX.
#endif
}

int main() { // Función principal que coordina el flujo global de ejecución del programa.
    std::cout << "========================================================\n"; // Imprime la línea divisoria superior.
    std::cout << "  ANALISIS DE SISTEMAS DINAMICOS (LINEALES Y NO LINEALES)\n"; // Imprime el título del programa.
    std::cout << "========================================================\n\n"; // Imprime la línea divisoria inferior.

    int option = 0; // Declara la variable para almacenar la selección del menú elegida por el usuario.
    std::cout << "Seleccione el tipo de sistema a evaluar:\n"; // Solicita la elección del tipo de sistema.
    std::cout << "  1. Sistema Lineal   (x' = a1*x + b1*y,  y' = a2*x + b2*y)\n"; // Muestra la opción de sistema lineal.
    std::cout << "  2. Sistema No Lineal (x' = f(x,y),       y' = g(x,y))\n"; // Muestra la opción de sistema no lineal.
    std::cout << "Opcion (1 o 2): "; // Solicita el ingreso de la opción por consola.
    if (!(std::cin >> option)) { // Detecta entrada no numérica o vacía en el menú (evita usar basura en 'option').
        std::cerr << "Error: entrada no valida (no numerica o vacia). Saliendo.\n"; // Mensaje de error claro.
        return 1; // Retorna código de error 1.
    } // Fin de la validación del menú.
    std::cout << "\n"; // Añade un salto de línea.

    SystemAnalyzer analyzer; // Instancia el objeto clasificador cualitativo de la dinámica.
    TrajectoryExporter exporter; // Instancia el exportador y simulador numérico mediante Runge-Kutta 4.
    std::string csvFilename = "trajectories.csv"; // Define el nombre estándar del archivo CSV de salida.
    bool exportSuccess = false; // Bandera de control para verificar si la exportación a CSV fue exitosa.
    bool skippedByDesign = false; // true => se omite la simulación a propósito (punto crítico no aislado).

    if (option == 1) { // Caso 1: El usuario seleccionó analizar un sistema lineal.
        double a1, b1, a2, b2; // Declara las variables para los coeficientes de la matriz del sistema lineal.
        std::cout << "--- Ingrese coeficientes del Sistema Lineal ---\n"; // Título de la sección lineal.
        std::cout << "  a1: "; std::cin >> a1; // Solicita y captura a1.
        std::cout << "  b1: "; std::cin >> b1; // Solicita y captura b1.
        std::cout << "  a2: "; std::cin >> a2; // Solicita y captura a2.
        std::cout << "  b2: "; std::cin >> b2; // Solicita y captura b2.
        std::cout << "\n"; // Salto de línea decorativo.

        if (!std::cin) { // Algún coeficiente no era numérico (p.ej. letra o entrada vacía).
            std::cerr << "Error: entrada no valida (no numerica o vacia). Saliendo.\n"; // Mensaje de error claro.
            return 1; // Retorna código de error 1 sin analizar basura.
        } // Fin de la validación de tipo numérico.
        if (!std::isfinite(a1) || !std::isfinite(b1) || !std::isfinite(a2) || !std::isfinite(b2)) { // Rechaza NaN/inf.
            std::cerr << "Error: entrada no valida (NaN o infinito). Saliendo.\n"; // Mensaje de error claro.
            return 1; // Retorna código de error 1.
        } // Fin de la validación de finitud.

        Matrix2D systemMatrix(a1, b1, a2, b2); // Instancia la matriz de 2x2 asociada al sistema lineal.
        AnalysisResult result = analyzer.analyze(systemMatrix); // Realiza el análisis analítico de estabilidad y punto crítico.
        analyzer.printReport(result); // Imprime el informe detallado en pantalla.

        if (result.isUnique) { // Solo simula si el punto crítico es aislado/único.
            exportSuccess = exporter.exportSimulationData(systemMatrix, csvFilename); // Ejecuta la simulación RK4 lineal.
        } else { // Punto no único: graficar "alrededor del origen" seria engañoso.
            skippedByDesign = true; // Marca la omisión intencional para no reportarla como error.
            std::cout << "\n[AVISO] Se omite la simulacion y el retrato de fase: el origen no es un punto critico aislado.\n"; // Aviso explicito.
        } // Fin de la condición de unicidad.
    } else if (option == 2) { // Caso 2: El usuario seleccionó analizar un sistema no lineal.
        std::cout << "--- Sistema No Lineal Cuadratico General con Terminos Cruzados ---\n"; // Encabezado de la opción no lineal.
        std::cout << "Modelo: x' = a1*x + b1*y + c1*x^2 + d1*y^2 + e1*x*y\n"; // Muestra el modelo matemático expandido para x'.
        std::cout << "        y' = a2*x + b2*y + c2*x^2 + d2*y^2 + e2*x*y\n\n"; // Muestra el modelo matemático expandido para y'.

        double a1, b1, c1, d1, e1; // Declara las variables para los 5 coeficientes de la primera ecuación x' = f(x,y).
        double a2, b2, c2, d2, e2; // Declara las variables para los 5 coeficientes de la segunda ecuación y' = g(x,y).

        std::cout << "Ingrese coeficientes para x' (a1 b1 c1 d1 e1): "; // Solicita ingresar los 5 coeficientes de f(x,y).
        std::cin >> a1 >> b1 >> c1 >> d1 >> e1; // Captura a1, b1, c1, d1, e1 separados por espacios.

        std::cout << "Ingrese coeficientes para y' (a2 b2 c2 d2 e2): "; // Solicita ingresar los 5 coeficientes de g(x,y).
        std::cin >> a2 >> b2 >> c2 >> d2 >> e2; // Captura a2, b2, c2, d2, e2 separados por espacios.
        std::cout << "\n"; // Salto de línea decorativo.

        if (!std::cin) { // Algún coeficiente no era numérico (p.ej. letra o entrada vacía).
            std::cerr << "Error: entrada no valida (no numerica o vacia). Saliendo.\n"; // Mensaje de error claro.
            return 1; // Retorna código de error 1 sin analizar basura.
        } // Fin de la validación de tipo numérico.
        if (!std::isfinite(a1) || !std::isfinite(b1) || !std::isfinite(c1) || !std::isfinite(d1) || !std::isfinite(e1) ||
            !std::isfinite(a2) || !std::isfinite(b2) || !std::isfinite(c2) || !std::isfinite(d2) || !std::isfinite(e2)) { // Rechaza NaN/inf en los 10 coeficientes.
            std::cerr << "Error: entrada no valida (NaN o infinito). Saliendo.\n"; // Mensaje de error claro.
            return 1; // Retorna código de error 1.
        } // Fin de la validación de finitud.

        // Define f(x,y) mediante una expresión lambda que incluye términos lineales, cuadráticos y el término cruzado e1*x*y.
        auto f = [a1, b1, c1, d1, e1](double x, double y) {
            return a1 * x + b1 * y + c1 * x * x + d1 * y * y + e1 * x * y;
        }; // Fin de la expresión lambda para f(x,y).

        // Define g(x,y) mediante una expresión lambda que incluye términos lineales, cuadráticos y el término cruzado e2*x*y.
        auto g = [a2, b2, c2, d2, e2](double x, double y) {
            return a2 * x + b2 * y + c2 * x * x + d2 * y * y + e2 * x * y;
        }; // Fin de la expresión lambda para g(x,y).

        NonLinearSystem nonLinearSys(f, g); // Instancia el sistema no lineal pasando las funciones f y g.
        AnalysisResult result = analyzer.analyzeNonLinear(nonLinearSys, 0.0, 0.0); // Evalúa el Jacobiano en (0,0) y verifica la hiperbolicidad (Hartman-Grobman).
        analyzer.printReport(result); // Imprime el diagnóstico del punto de equilibrio en pantalla.

        if (result.isUnique) { // Solo simula si (0,0) es un punto crítico de equilibrio válido.
            exportSuccess = exporter.exportSimulationDataNonLinear(nonLinearSys, csvFilename); // Ejecuta la integración numérico-temporal RK4 no lineal.
        } else { // (0,0) no es crítico o no es aislado: no tiene sentido retrato alrededor del origen.
            skippedByDesign = true; // Marca la omisión intencional para no reportarla como error.
            std::cout << "\n[AVISO] Se omite la simulacion y el retrato de fase: el origen no es un punto critico aislado.\n"; // Aviso explicito.
        } // Fin de la condición de unicidad.
    } else { // Manejo de selección inválida.
        std::cout << "Opcion no valida. Saliendo del programa.\n"; // Notifica la entrada incorrecta.
        return 1; // Retorna código de error 1.
    } // Fin del bloque condicional de opciones.

    if (exportSuccess) { // Evalúa si el archivo CSV con las trayectorias se generó correctamente.
        std::cout << "\nEjecutando script de visualizacion grafica en Python...\n"; // Notifica el llamado al módulo de gráficos.
        std::string command = findPython() + " scripts/plot_phase_portrait.py " + csvFilename; // Construye la instrucción de consola usando el Python detectado (venv primero).
        int plotStatus = std::system(command.c_str()); // Ejecuta el comando del sistema operativo y captura su código de salida.
        if (plotStatus != 0) { // Verifica si el script de gráficos terminó con error.
            std::cerr << "Advertencia: fallo el script de graficos (codigo " << plotStatus << ").\n"; // Informa que el retrato de fase no se genero.
            std::cerr << "Verifica que el entorno virtual tenga matplotlib y pandas instalados.\n"; // Sugerencia concreta de solucion.
        } else { // El script termino correctamente.
            std::cout << "[OK] Retrato de fase generado en phase_portrait.png\n"; // Confirma la generacion de la imagen.
        } // Fin de la verificación del código de retorno.
    } else { // En caso de fallo REAL en la simulación (la omisión intencional no es un error).
        if (!skippedByDesign) { // Solo reporta error si la omisión no fue a proposito.
            std::cerr << "Error: Ocurrio un fallo al generar o exportar la simulacion numerica.\n"; // Reporta el error en el flujo de salida de errores std::cerr.
        } // Fin de la proteccion contra la omision intencional.
    } // Fin de la verificación de exportación.

    return 0; // Finaliza la ejecución del programa sin errores.
} // Fin de la función principal main.