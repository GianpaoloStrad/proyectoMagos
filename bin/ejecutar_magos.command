#!/bin/bash

# Obtener el directorio donde está este script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Cambiar al directorio del script
cd "$SCRIPT_DIR"

# Ejecutar el programa
./magos

# Mantener la terminal abierta si hay algún error
if [ $? -ne 0 ]; then
    echo "Presiona Enter para cerrar..."
    read
fi 