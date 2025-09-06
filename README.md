# granular_render

Programa que renderiza salidas de coordenadas generadas por simulaciones granulares con Box2D.

Contiene también dos scripts:
- `getPythonRGBColors.py`: obtiene las componentes RGB de los colores *Tableau Palette* de Matplotlib.
- `dovideo.sh`: genera una animación a partir de los png generados por `granular_render`

## Construir el ejecutable

```
mkdir build
cd build
cmake ..
make 
```
## Uso 

    ./granular_render <directorio con *.xy> nthreads xmin xmax ymin ymax [margin]

Ejemplo: 

    ./granular_render ./frames 8 -10 10 -5 30 0.05

# Formato de archivo xy 

El programa lee archivos de texto con extensión `.xy` que tiene el siguiente formato:

```
# linea de comentario (puede contener el timestep o tiempo de simulación)
id nv v1x v1y v2x v2y ... vnvx vnvy type
...
id 1 x y r type
...
```
Cada grano tiene un `id` entero positivo, `nv` es el número de vértices que tiene el polígono 
que representa al grano, y `type` es un entero que denota el tipo de grano.

Cuando `nv = 1`, la línea representa un grano circular o disco, `x` y `y` son las coordenadas
del centro del disco, y `r` su radio.

Las paredes de los contenedores son también poligonales, pero con `id < 0`.
