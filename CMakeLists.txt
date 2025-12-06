cmake_minimum_required(VERSION 3.10)
project(granular_render LANGUAGES CXX)

# Configurar directorios de salida
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# Estándar de C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar Cairo
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED cairo)

# Incluir directorios
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CAIRO_INCLUDE_DIRS}
)

# Archivos fuente
set(SOURCES
    src/main.cpp
    src/renderer.cpp
    src/thread_pool.cpp
)

# Ejecutable
add_executable(granular_render ${SOURCES})

# Configuraciones específicas por modo de build
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # Flags comunes para GCC y Clang
    target_compile_options(granular_render PRIVATE
        # Warnings comunes
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
    )
    
    # Flags específicos para Debug
    target_compile_options(granular_render PRIVATE
        $<$<CONFIG:Debug>:
            -g
            -O0
            -fno-omit-frame-pointer
            -fsanitize=address
            -fsanitize=undefined
            -fno-optimize-sibling-calls
            -DDEBUG
            -D_DEBUG
        >
    )
    
    # Flags específicos para Release
    target_compile_options(granular_render PRIVATE
        $<$<CONFIG:Release>:
            -O3
            -DNDEBUG
            -march=native
            -ffast-math
            -fno-trapping-math
        >
    )
    
    # Flags específicos para MSVC
elseif(MSVC)
    target_compile_options(granular_render PRIVATE
        /W4
        /wd4201  # nonstandard extension used: nameless struct/union
        /wd4100  # unreferenced formal parameter
    )
    
    target_compile_options(granular_render PRIVATE
        $<$<CONFIG:Debug>:
            /Zi
            /Od
            /RTC1
            /DDEBUG
            /D_DEBUG
        >
    )
    
    target_compile_options(granular_render PRIVATE
        $<$<CONFIG:Release>:
            /O2
            /Ob2
            /DNDEBUG
            /GL
        >
    )
endif()

# También puedes definir variables para usar en el código
target_compile_definitions(granular_render
    PRIVATE
        $<$<CONFIG:Debug>:GRANULAR_DEBUG=1>
        $<$<CONFIG:Release>:GRANULAR_RELEASE=1>
)

# Vincular librerías
target_link_libraries(granular_render
    ${CAIRO_LIBRARIES}
)

# Para Release, agregar flags de link-time optimization si está disponible
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_link_options(granular_render
        PRIVATE
            $<$<CONFIG:Release>:-flto>
    )
endif()

# Flags extra para Cairo
target_compile_options(granular_render PRIVATE ${CAIRO_CFLAGS_OTHER})

# Configurar propiedades del objetivo
set_target_properties(granular_render PROPERTIES
    CXX_EXTENSIONS OFF
    # Asegurar que se recompile cuando cambien los flags
    # COMPILE_FLAGS "-Werror"
)

# Mensaje informativo
message(STATUS "Configuración de compilación:")
message(STATUS "  - Modo: ${CMAKE_BUILD_TYPE}")
message(STATUS "  - Compilador: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  - Flags Debug: -g -O0 -fsanitize=address -DDEBUG")
message(STATUS "  - Flags Release: -O3 -march=native -ffast-math -DNDEBUG")
