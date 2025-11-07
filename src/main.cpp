#include "config.hpp"
#include "renderer.hpp"
#include "thread_pool.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

// Función para mostrar ayuda
void show_help(const std::string& program_name) {
    std::cout << "Uso: " << program_name << " [OPCIONES]\n\n";
    std::cout << "Opciones obligatorias:\n";
    std::cout << "  -d, --dir DIRECTORIO    Directorio con archivos *.xy\n";
    std::cout << "  -x, --xylimits XMIN XMAX YMIN YMAX\n";
    std::cout << "                          Límites del espacio de simulación\n\n";
    std::cout << "Opciones opcionales:\n";
    std::cout << "  -t, --threads N         Número de hilos (default: todos los cores)\n";
    std::cout << "  -W, --width W           Ancho de la imagen (default: " << DEFAULT_WIDTH << ")\n";
    std::cout << "  -H, --height H          Alto de la imagen (default: " << DEFAULT_HEIGHT << ")\n";
    std::cout << "  -m, --margin M          Margen alrededor (default: " << DEFAULT_MARGIN << ")\n";
    std::cout << "  -h, --help              Mostrar esta ayuda\n\n";
    std::cout << "Ejemplos:\n";
    std::cout << "  " << program_name << " -d ./frames -x -10 10 -5 30\n";
    std::cout << "  " << program_name << " --dir frames --xylimits -5 5 -5 5 --width 1920 --height 1080\n";
    std::cout << "  " << program_name << " -d frames -x -8 8 -2 25 -t 4 -W 1200 -H 800 -m 0.02\n";
}

// Función para parsear argumentos
bool parse_arguments(int argc, char** argv, 
                    std::string& dir,
                    unsigned int& nthreads,
                    double& xmin, double& xmax, double& ymin, double& ymax,
                    int& width, int& height,
                    double& margin) {
    
    // Valores por defecto
    dir = "";
    nthreads = 0;
    xmin = xmax = ymin = ymax = 0.0;
    width = DEFAULT_WIDTH;
    height = DEFAULT_HEIGHT;
    margin = DEFAULT_MARGIN;
    
    std::vector<std::string> args(argv + 1, argv + argc);
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "-h" || args[i] == "--help") {
            show_help(argv[0]);
            return false;
        }
        else if (args[i] == "-d" || args[i] == "--dir") {
            if (i + 1 < args.size()) {
                dir = args[++i];
            } 
            else {
                std::cerr << "Error: Falta el directorio después de " << args[i] << "\n";
                return false;
            }
        }
        else if (args[i] == "-t" || args[i] == "--threads") {
            if (i + 1 < args.size()) {
                nthreads = static_cast<unsigned int>(std::stoi(args[++i]));
            } 
            else {
                std::cerr << "Error: Falta el número de hilos después de " << args[i] << "\n";
                return false;
            }
        }
        else if (args[i] == "-x" || args[i] == "--xylimits") {
            if (i + 4 < args.size()) {
                xmin = std::stod(args[++i]);
                xmax = std::stod(args[++i]);
                ymin = std::stod(args[++i]);
                ymax = std::stod(args[++i]);
            } 
            else {
                std::cerr << "Error: Faltan valores después de " << args[i] << "\n";
                return false;
            }
        }
        else if (args[i] == "-W" || args[i] == "--width") {
            if (i + 1 < args.size()) {
                width = std::stoi(args[++i]);
                if (width <= 0) {
                    std::cerr << "Error: El ancho debe ser positivo\n";
                    return false;
                }
            } else {
                std::cerr << "Error: Falta el ancho después de " << args[i] << "\n";
                return false;
            }
        }
        else if (args[i] == "-H" || args[i] == "--height") {
            if (i + 1 < args.size()) {
                height = std::stoi(args[++i]);
                if (height <= 0) {
                    std::cerr << "Error: El alto debe ser positivo\n";
                    return false;
                }
            } else {
                std::cerr << "Error: Falta el alto después de " << args[i] << "\n";
                return false;
            }
        }
        else if (args[i] == "-m" || args[i] == "--margin") {
            if (i + 1 < args.size()) {
                margin = std::stod(args[++i]);
                if (margin < 0) {
                    margin = 0.0;
                    std::cerr << "Warning: Margen negativo convertido a 0\n";
                }
            } else {
                std::cerr << "Error: Falta el margen después de " << args[i] << "\n";
                return false;
            }
        }
        else {
            std::cerr << "Error: Argumento desconocido: " << args[i] << "\n";
            show_help(argv[0]);
            return false;
        }
    }
    
    // Validar parámetros obligatorios
    if (dir.empty()) {
        std::cerr << "Error: Se debe especificar un directorio con -d/--dir\n";
        return false;
    }
    
    if (xmin >= xmax || ymin >= ymax) {
        std::cerr << "Error: Límites inválidos. Debe ser: xmin < xmax, ymin < ymax\n";
        return false;
    }
    
    return true;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        show_help(argv[0]);
        return 1;
    }
    
    std::string dir;
    unsigned int nthreads;
    double xmin, xmax, ymin, ymax;
    int width, height;
    double margin;
    
    if (!parse_arguments(argc, argv, dir, nthreads, xmin, xmax, ymin, ymax, width, height, margin)) {
        return 1;
    }
    
    // Configurar número de hilos por defecto si no se especificó
    if (nthreads == 0) {
        nthreads = std::thread::hardware_concurrency();
        if (nthreads == 0) {
            nthreads = 2;
            std::cerr << "Warning: No se pudo detectar el número de cores, usando 2 hilos\n";
        }
    }
    
    std::cout << "Configuración:\n";
    std::cout << "  Directorio: " << dir << "\n";
    std::cout << "  Hilos: " << nthreads << "\n";
    std::cout << "  Límites: x=[" << xmin << ", " << xmax << "] y=[" << ymin << ", " << ymax << "]\n";
    std::cout << "  Tamaño: " << width << "x" << height << "\n";
    std::cout << "  Margen: " << margin << "\n\n";
    
    // Verificar que el directorio existe
    if (!fs::exists(dir)) {
        std::cerr << "Error: El directorio '" << dir << "' no existe\n";
        return 1;
    }
    
    ThreadPool pool(nthreads);
    int processed_files = 0;
    
    for (auto& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension() == ".xy") {
            std::string inputFile = entry.path().string();
            auto outPath = entry.path();
            outPath.replace_extension(".png");
            std::string outputFile = outPath.string();
            
            pool.enqueue([=, &processed_files]() {
                try {
                    renderFile(inputFile, outputFile, width, height, xmin, xmax, ymin, ymax, margin);
                    std::cout << "Procesado: " << entry.path().filename() << " -> " << outPath.filename() << "\n";
                } catch (const std::exception& e) {
                    std::cerr << "Error procesando " << entry.path().filename() << ": " << e.what() << "\n";
                }
                processed_files++;
            });
        }
    }
    
    // Esperar a que terminen todos los trabajos
    pool.wait_all();
    
    std::cout << "\nProcesamiento completado. Archivos procesados: " << processed_files << "\n";
    
    return 0;
}
