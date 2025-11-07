#include "renderer.hpp"
#include "config.hpp"
#include <cairo/cairo.h>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

// ---------------------
// Estructuras
// ---------------------
struct Circle {
  double x, y, r;
  int type;
  int id;
};

struct Polygon {
  std::vector<std::pair<double, double>> vertices;
  int type;
  int id;
};

struct Line {
    std::vector<std::pair<double, double>> endpoints;
    int id;
};

// ---------------------
// Colores por tipo
// ---------------------
static void setColorForType(cairo_t *cr, int type) {
  auto [r, g, b] = COLORS[static_cast<size_t>(type) % COLORS.size()];
  cairo_set_source_rgb(cr, r, g, b);
}

// ---------------------
// Función principal de render
// ---------------------
void renderFile(const std::string &inputFile, const std::string &outputFile,
                int width, int height, double xmin, double xmax, double ymin,
                double ymax, double margin) {
  std::ifstream in(inputFile);
  if (!in) {
    std::cerr << "No se pudo abrir " << inputFile << "\n";
    return;
  }

  std::vector<Circle> circles;
  std::vector<Polygon> polygons;
  std::vector<Line> lines;
  std::string line;

  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream iss(line);
    int id, nvert, type;
    iss >> id >> nvert;

    if (nvert == 1) {
      double x, y, r;
      iss >> x >> y >> r >> type;
      circles.push_back({x, y, r, type, id});
    } 
    else if (nvert == 2) {
        std::vector<std::pair<double, double>> endpts;
        double p1x, p1y, p2x, p2y;
        iss >> p1x >> p1y >> p2x >> p2y;
        endpts.push_back({p1x, p1y});
        endpts.push_back({p2x, p2y});
        lines.push_back({endpts, id});
    } 
    else {
      std::vector<std::pair<double, double>> verts;
      for (int i = 0; i < nvert; i++) {
        double vx, vy;
        iss >> vx >> vy;
        verts.push_back({vx, vy});
      }
      iss >> type;
      polygons.push_back({verts, type, id});
    }
  }

  cairo_surface_t *surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
  cairo_t *cr = cairo_create(surface);

  // Fondo blanco
  cairo_set_source_rgb(cr, 1, 1, 1);
  cairo_paint(cr);

  // ---------------------
  // Margen configurable
  // ---------------------
  double xrange = xmax - xmin;
  double yrange = ymax - ymin;

  xmin -= xrange * margin;
  xmax += xrange * margin;
  ymin -= yrange * margin;
  ymax += yrange * margin;

  // ---------------------
  // Escala y centrado
  // ---------------------
  double scale_x = width / (xmax - xmin);
  double scale_y = height / (ymax - ymin);
  double scale = std::min(scale_x, scale_y);

  double sim_w = (xmax - xmin) * scale;
  double sim_h = (ymax - ymin) * scale;

  double offset_x = (width - sim_w) / 2.0;
  double offset_y = (height - sim_h) / 2.0;

  cairo_translate(cr, offset_x, height - offset_y);
  cairo_scale(cr, scale, -scale);
  cairo_translate(cr, -xmin, -ymin);

  // ---------------------
  // Dibujar polígonos
  // ---------------------
  for (auto &poly : polygons) {
    if (poly.vertices.empty())
      continue;
    cairo_move_to(cr, poly.vertices[0].first, poly.vertices[0].second);
    for (size_t i = 1; i < poly.vertices.size(); i++)
      cairo_line_to(cr, poly.vertices[i].first, poly.vertices[i].second);

    if (poly.id < 0) {
      // Contenedor
      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_fill_preserve(cr);
      cairo_set_line_width(cr, BORDER_WIDTH_CONTAINER);
      cairo_stroke(cr);
      cairo_close_path(cr);
    } else {
      // Grano
      setColorForType(cr, poly.type);
      cairo_fill_preserve(cr);
      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_set_line_width(cr, BORDER_WIDTH_GRAIN);
      cairo_stroke(cr);
      cairo_close_path(cr);
    }
  }

  // ---------------------
  // Dibujar líneas
  // ---------------------
  for (auto &lin : lines) {
    if (lin.endpoints.empty())
      continue;
    cairo_move_to(cr, lin.endpoints[0].first, lin.endpoints[0].second);
    cairo_line_to(cr, lin.endpoints[1].first, lin.endpoints[1].second);
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_fill_preserve(cr);
    cairo_set_source_rgb(cr, 0, 0, 0);
    cairo_set_line_width(cr, BORDER_WIDTH_CONTAINER);
    cairo_stroke(cr);
  }


  // ---------------------
  // Dibujar círculos
  // ---------------------
  for (auto &c : circles) {
    cairo_arc(cr, c.x, c.y, c.r, 0, 2 * M_PI);
    if (c.id < 0) {
      // Contenedor circular
      cairo_set_source_rgb(cr, 1, 1, 1);
      cairo_fill_preserve(cr);
      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_set_line_width(cr, BORDER_WIDTH_CONTAINER);
      cairo_stroke(cr);
    } else {
      // Grano circular
      setColorForType(cr, c.type);
      cairo_fill_preserve(cr);
      cairo_set_source_rgb(cr, 0, 0, 0);
      cairo_set_line_width(cr, BORDER_WIDTH_GRAIN);
      cairo_stroke(cr);
    }
  }

  cairo_destroy(cr);
  cairo_surface_write_to_png(surface, outputFile.c_str());
  cairo_surface_destroy(surface);
}
