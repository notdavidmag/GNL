# Get Next Line

*Este proyecto ha sido creado como parte del currículo de 42 por damagda.*

## Descripción

**Get Next Line** es una función en C que lee y devuelve una línea completa desde un descriptor de archivo (`fd`) cada vez que es llamada. Permite leer cualquier archivo o la entrada estándar (`stdin`) línea por línea, de forma eficiente y sin cargar el fichero entero en memoria.

- Devuelve la línea leída (incluyendo el `\n` final si existe).
- Devuelve `NULL` al llegar al final del archivo o si ocurre un error.
- El tamaño del buffer de lectura se configura en tiempo de compilación con `-D BUFFER_SIZE=n`.

## Instrucciones

### Compilación

```bash
cc -Wall -Werror -Wextra -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c
```

Puedes cambiar `42` por cualquier valor positivo. Si no se especifica `BUFFER_SIZE`, el valor por defecto es `42`.

### Con Makefile (programa de prueba)

```bash
make                  # compila con BUFFER_SIZE=42 por defecto
make BUFFER=1         # compila con BUFFER_SIZE=1
make fclean           # limpia todo
make re               # recompila desde cero
```

### Uso en tu propio código

```c
#include "get_next_line.h"

int main(void)
{
    int     fd;
    char    *line;

    fd = open("archivo.txt", O_RDONLY);
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}
```

## Algoritmo

### Descripción del enfoque

El algoritmo se basa en una **variable estática** (`stash`) que persiste entre llamadas a `get_next_line`. Esto permite acumular los datos leídos del archivo entre invocaciones sucesivas sin perder el contexto.

### Flujo de ejecución

1. **Lectura en chunks**: Se llama a `read()` repetidamente con un buffer de tamaño `BUFFER_SIZE`, acumulando el resultado en `stash` mediante `ft_strjoin_gnl`.
2. **Detección de línea**: Se para de leer en cuanto se detecta un `\n` en el stash (o se llega a EOF).
3. **Extracción**: `extract_line()` recorta el stash desde el inicio hasta el primer `\n` (incluido) y devuelve esa subcadena como la línea.
4. **Actualización del stash**: `update_stash()` elimina la línea ya devuelta del stash, dejando el resto disponible para la siguiente llamada.

### Justificación

Este enfoque es eficiente porque:
- **Lee lo mínimo necesario**: en cuanto encuentra un `\n`, deja de llamar a `read()`.
- **Funciona con cualquier `BUFFER_SIZE`**: desde 1 hasta 10.000.000, el stash siempre acumula correctamente.
- **No usa variables globales**: la variable estática está encapsulada dentro de `get_next_line`.
- **Gestiona correctamente EOF sin `\n`**: si el archivo no termina en salto de línea, se devuelve el último fragmento igualmente.

### Por qué variable estática y no global

Una variable global sería accesible desde cualquier parte del programa, lo que rompe el encapsulamiento y está explícitamente prohibido. La variable estática local solo existe en el scope de `get_next_line` pero mantiene su valor entre llamadas, lo que es exactamente lo que se necesita.

## Recursos

- [man 2 read](https://man7.org/linux/man-pages/man2/read.2.html) — Documentación oficial de la syscall `read`.
- [man 3 malloc / free](https://man7.org/linux/man-pages/man3/malloc.3.html) — Gestión de memoria dinámica en C.
- [Static variables in C — GeeksForGeeks](https://www.geeksforgeeks.org/static-variables-in-c/) — Explicación clara de variables estáticas.
- [File descriptors — Wikipedia](https://en.wikipedia.org/wiki/File_descriptor) — Concepto de descriptor de archivo.

### Uso de IA

Se utilizó Claude (Anthropic) como herramienta de apoyo durante el desarrollo del proyecto. Concretamente:
- **Generación del código base**: se usó para producir una primera versión de los tres archivos `.c` y `.h`.
- **Revisión de Norminette**: se verificó que el código cumpliera las restricciones de estilo de 42 (máx. 25 líneas por función, sin `for`, variables al inicio del bloque, etc.).
- **Documentación**: este README fue redactado con asistencia de IA.

El código fue revisado y comprendido manualmente antes de su entrega.
