#!/bin/bash
# dovideo.sh
# Script para concatenar imágenes secuenciales para armar una animación
# Manuel Carlevaro - 2021.07.26

if [ "$#" -ne 4 ]; then
    echo "Uso: "
    echo "  ./dovideo directorio prefijo nombre_video frame_rate"
    echo "  "
    echo "      directorio: donde están los archivos de imagen"
    echo "      prefijo: prefijo de los archivos de imagen"
    echo "      nombre_video sin extensión (agrega .mp4)"
    echo "      frame_rate sugeridos: 20, 25, 30."
    echo "  "
    echo "Ejemplo: "
    echo "  ./dovideo frames frm anim 25"
    echo "  "
    exit 1
fi

if ! [[ "$4" =~ ^[0-9]+$ ]]; then
    echo "El cuarto argumento debe ser un entero." >&2
    exit 1
fi

DIR="$1"
PREFIX="$2"
VIDEO_NAME="$3"
FRAME_RATE="$4"

# Cambiar al directorio si existe
cd "$DIR" || exit 1

ffmpeg -framerate $FRAME_RATE -pattern_type glob -i "${PREFIX}_*.png" \
    -c:v libx264 -pix_fmt yuv420p \
    -crf 23 -preset medium \
    ../${VIDEO_NAME}.mp4
