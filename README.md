
# Bus Evacuation Problem (BEP)

Este proyecto implementa una solución heurística para el Bus Evacuation Problem (BEP), combinando un algoritmo Greedy para construir una solución inicial y Hill Climbing para mejorarla localmente.

El objetivo del problema es evacuar personas desde puntos de encuentro hacia refugios, minimizando el makespan (el tiempo que tarda el bus más lento en completar todos sus viajes), respetando restricciones de capacidad de buses y refugios.

## Requisitos

- Entorno Linux (puede ser WSL en Windows).
- Compilador de C++ (por ejemplo, g++) instalado y accesible desde la terminal.

## Compilación

Este proyecto incluye un Makefile que permite compilar el programa fácilmente.

1. Limpiar compilaciones anteriores:

   ```bash
   make clean
   ```

2. Compilar el programa:

   ```bash
   make
   ```

Esto genera el archivo ejecutable `BEP_HC_Greedy.exe`.

## Ejecución

Para ejecutar el programa, utiliza por ejemplo:

```bash
./BEP_HC_Greedy.exe InstanceBEP-1-4-2-4.txt
```

> Asegúrate de que el archivo de instancia esté en el mismo directorio que el ejecutable, o especifica la ruta completa al archivo.

## Descripción del algoritmo

* **Greedy**: Construye una solución inicial evacuando siempre desde el punto con más personas al refugio más cercano disponible.
* **Hill Climbing**: Intenta mejorar la solución redistribuyendo viajes del bus más lento al más rápido, siempre que se reduzca el makespan.


## Consideraciones

* El ejecutable generado es `.exe`, pero puede ejecutarse normalmente en Linux si estás usando WSL o has montado un entorno tipo Windows (por compatibilidad).
* El programa imprime en consola los viajes de cada bus y el makespan final de la solución obtenida por cada algoritmo. Además, muestra el tiempo de ejecución total en milisegundos.
* El archivo de instancia debe seguir el formato esperado por el código fuente.

