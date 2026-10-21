cmake_minimum_required(VERSION 3.12)
project(SistemasDinamicos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCE_FILES
    src/main.cpp
    src/Matrix2D.cpp
    src/NonLinearSystem.cpp
    src/SystemAnalyzer.cpp
    src/TrajectoryExporter.cpp
)

add_executable(SistemasDinamicos ${SOURCE_FILES})
target_include_directories(SistemasDinamicos PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Copia la carpeta scripts al directorio build para la ejecución automática
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/scripts DESTINATION ${CMAKE_CURRENT_BINARY_DIR})