#pragma once
#include <string>

void renderFile(const std::string &inputFile, const std::string &outputFile,
                int width, int height, double xmin, double xmax, double ymin,
                double ymax,
                double margin); // 👈 agregamos margin aquí
