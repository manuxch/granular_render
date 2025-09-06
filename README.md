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

Ejemplo: ./granular_render ./frames 8 -10 10 -5 30 0.05

