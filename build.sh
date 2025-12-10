#!/bin/bash
# build.sh - Script para compilar el Graficador de Sistemas Granulares
# Uso: ./build.sh [opciones]

set -e # Salir en error

# Configuración por defecto
BUILD_TYPE="Release"
BUILD_DIR="build"
CLEAN_BUILD=false
SHOW_HELP=false
VERBOSE=false
JOBS=$(nproc)
EXECUTABLE_NAME="granular_render"

# Detectar si soporta colores
if [ -t 1 ]; then
    HAS_COLOR=true
else
    HAS_COLOR=false
fi

# Función para mostrar ayuda
show_help() {
    cat <<'EOF'
Graficador de Sistemas Granulares - Script de Compilación

Uso:
    ./build.sh [opciones]

Opciones:
    -d, --debug          Compilar en modo Debug (por defecto: Release)
    -c, --clean          Limpiar directorio de build antes de compilar
    -j, --jobs N         Usar N jobs paralelos (por defecto: todos los cores)
    -v, --verbose        Mostrar output detallado
    -h, --help           Mostrar este mensaje de ayuda

Ejemplos:
    ./build.sh                    # Compilación normal Release
    ./build.sh --debug            # Compilación Debug
    ./build.sh --clean --debug    # Limpiar y compilar en Debug
    ./build.sh --jobs 4           # Usar 4 cores para compilar
    ./build.sh --verbose          # Output detallado

Descripción:
    Este script automatiza la compilación del programa que visualiza sistemas granulares
    utilizando Box2D para la simulación física y Cairo para el renderizado gráfico.

    El proyecto utiliza CMake, C++20, Box2D y Cairo para crear visualizaciones
    de simulaciones de partículas y cuerpos rígidos.

Características:
    - Simulación física con Box2D 3.2.0
    - Renderizado gráfico con Cairo
    - Soporte para cuerpos circulares y poligonales

Repositorio: https://github.com/manuxch/granular_render
EOF
}

# Funciones de logging
log_info() {
    if [ "$HAS_COLOR" = true ]; then
        printf "\033[34m[INFO]\033[0m %s\n" "$1"
    else
        printf "[INFO] %s\n" "$1"
    fi
}

log_success() {
    if [ "$HAS_COLOR" = true ]; then
        printf "\033[32m[SUCCESS]\033[0m %s\n" "$1"
    else
        printf "[SUCCESS] %s\n" "$1"
    fi
}

log_warning() {
    if [ "$HAS_COLOR" = true ]; then
        printf "\033[33m[WARNING]\033[0m %s\n" "$1"
    else
        printf "[WARNING] %s\n" "$1"
    fi
}

log_error() {
    if [ "$HAS_COLOR" = true ]; then
        printf "\033[31m[ERROR]\033[0m %s\n" "$1"
    else
        printf "[ERROR] %s\n" "$1"
    fi
}

# Verificar dependencias
check_dependencies() {
    log_info "Verificando dependencias del sistema..."

    # Verificar CMake
    if ! command -v cmake &>/dev/null; then
        log_error "CMake no está instalado. Por favor instálalo:"
        echo "  Ubuntu/Debian: sudo apt install cmake"
        echo "  Fedora: sudo dnf install cmake"
        echo "  macOS: brew install cmake"
        exit 1
    fi

    # Verificar pkg-config
    if ! command -v pkg-config &>/dev/null; then
        log_error "pkg-config no está instalado. Por favor instálalo:"
        echo "  Ubuntu/Debian: sudo apt install pkg-config"
        echo "  Fedora: sudo dnf install pkg-config"
        echo "  macOS: brew install pkg-config"
        exit 1
    fi

    # Verificar Cairo
    if ! pkg-config --exists cairo; then
        log_error "Cairo no está instalado o no se encuentra."
        echo "Por favor instala las librerías de desarrollo de Cairo:"
        echo "  Ubuntu/Debian: sudo apt install libcairo2-dev"
        echo "  Fedora: sudo dnf install cairo-devel"
        echo "  macOS: brew install cairo"
        exit 1
    fi

    log_success "Todas las dependencias verificadas correctamente"
}

# Parsear argumentos
while [[ $# -gt 0 ]]; do
    case $1 in
    -d | --debug)
        BUILD_TYPE="Debug"
        shift
        ;;
    -c | --clean)
        CLEAN_BUILD=true
        shift
        ;;
    -j | --jobs)
        if [[ $2 =~ ^[0-9]+$ ]]; then
            JOBS="$2"
            shift 2
        else
            log_error "Número de jobs inválido: $2"
            exit 1
        fi
        ;;
    -v | --verbose)
        VERBOSE=true
        shift
        ;;
    -h | --help)
        SHOW_HELP=true
        shift
        ;;
    *)
        log_error "Argumento desconocido: $1"
        echo "Usa './build.sh --help' para ver las opciones disponibles."
        exit 1
        ;;
    esac
done

# Mostrar ayuda si se solicitó
if [ "$SHOW_HELP" = true ]; then
    show_help
    exit 0
fi

echo ""

# Verificar dependencias
check_dependencies

log_info "Iniciando compilación del Graficador de Sistemas Granulares..."
log_info "Modo: $BUILD_TYPE"
log_info "Jobs: $JOBS"
log_info "Directorio de build: $BUILD_DIR"

# Crear directorio bin si no existe
log_info "Creando directorio de salida..."
mkdir -p ../bin

# Limpiar build si se solicitó
if [ "$CLEAN_BUILD" = true ]; then
    log_info "Limpiando directorio de build..."
    rm -rf "$BUILD_DIR"
fi

# Crear directorio de build si no existe
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configurar CMake
log_info "Configurando CMake..."
if [ "$VERBOSE" = true ]; then
    cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
else
    if ! cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" .. >/dev/null 2>&1; then
        log_error "Error en la configuración de CMake. Ejecuta con --verbose para más detalles."
        exit 1
    fi
fi

# Compilar
log_info "Compilando proyecto..."
if [ "$VERBOSE" = true ]; then
    if cmake --build . --config "$BUILD_TYPE" --parallel "$JOBS"; then
        log_success "Compilación completada exitosamente!"
    else
        log_error "Error en la compilación."
        exit 1
    fi
else
    if cmake --build . --config "$BUILD_TYPE" --parallel "$JOBS" >/dev/null 2>&1; then
        log_success "Compilación completada exitosamente!"
    else
        log_error "Error en la compilación. Ejecuta con --verbose para más detalles."
        exit 1
    fi
fi

# Verificar que el ejecutable se creó en bin/
if [ -f "../bin/$EXECUTABLE_NAME" ]; then
    log_success "Ejecutable generado correctamente en bin/"
    echo ""
    log_info "Ejecutable: ../bin/$EXECUTABLE_NAME"
    log_info "Tamaño: $(du -h "../bin/$EXECUTABLE_NAME" | cut -f1)"

    # Mostrar información de librerías vinculadas
    echo ""
    log_info "Librerías vinculadas:"
    if command -v ldd &>/dev/null; then
        ldd "../bin/$EXECUTABLE_NAME" | grep -E "(cairo|box2d)" || true
    fi

    echo ""
    log_info "Para ejecutar:"
    echo "  cd ../bin && ./$EXECUTABLE_NAME"
    echo "  o"
    echo "  ../bin/$EXECUTABLE_NAME"

else
    log_error "El ejecutable no se generó correctamente en bin/"
    log_info "Buscando archivos en el directorio build:"
    ls -la
    log_info "Buscando archivos en el directorio bin:"
    ls -la ../bin/ 2>/dev/null || echo "Directorio bin no existe"
    exit 1
fi

# Mensaje final
echo ""
log_success "¡El Graficador de Sistemas Granulares está listo para usar!"
