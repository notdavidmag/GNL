
#include "get_next_line.h"
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

/* ─── Colores para la terminal ────────────────────────────────────────────── */
#define GREEN  "\033[0;32m"
#define RED    "\033[0;31m"
#define YELLOW "\033[0;33m"
#define CYAN   "\033[0;36m"
#define RESET  "\033[0m"

/* ─── Helpers ─────────────────────────────────────────────────────────────── */

/* Crea un archivo temporal con el contenido que le pases */
static void	create_test_file(const char *filename, const char *content)
{
	int	fd;

	fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd < 0)
	{
		printf(RED "[ERROR] No se pudo crear %s\n" RESET, filename);
		return ;
	}
	if (content)
		write(fd, content, strlen(content));
	close(fd);
}

/* Imprime una línea con caracteres no imprimibles visibles */
static void	print_visible(const char *label, const char *s)
{
	printf("  %s: [", label);
	if (!s)
	{
		printf("NULL]\n");
		return ;
	}
	while (*s)
	{
		if (*s == '\n')
			printf("\\n");
		else if (*s == '\0')
			printf("\\0");
		else
			putchar(*s);
		s++;
	}
	printf("]\n");
}

/* ─── Tests individuales ──────────────────────────────────────────────────── */

/*
** TEST 1 — Archivo normal con varias líneas
** Esperado: leer cada línea con \n al final, luego NULL
*/
static void	test_normal_file(void)
{
	int		fd;
	char	*line;
	int		i;

	printf(CYAN "\n[TEST 1] Archivo normal (varias líneas)\n" RESET);
	create_test_file("/tmp/gnl_t1.txt", "linea uno\nlinea dos\nlinea tres\n");
	fd = open("/tmp/gnl_t1.txt", O_RDONLY);
	i = 1;
	while (1)
	{
		line = get_next_line(fd);
		if (!line)
			break ;
		printf(GREEN "  Línea %d" RESET, i++);
		print_visible("", line);
		free(line);
	}
	printf("  Llamada extra → ");
	print_visible("", get_next_line(fd)); /* Debe ser NULL */
	close(fd);
}

/*
** TEST 2 — Archivo sin \n al final
** Esperado: última línea SIN \n, luego NULL
*/
static void	test_no_newline_at_end(void)
{
	int		fd;
	char	*line;
	int		i;

	printf(CYAN "\n[TEST 2] Archivo sin \\n al final\n" RESET);
	create_test_file("/tmp/gnl_t2.txt", "primera\nsegunda\ntercera sin newline");
	fd = open("/tmp/gnl_t2.txt", O_RDONLY);
	i = 1;
	while (1)
	{
		line = get_next_line(fd);
		if (!line)
			break ;
		printf(GREEN "  Línea %d" RESET, i++);
		print_visible("", line);
		free(line);
	}
	close(fd);
}

/*
** TEST 3 — Archivo vacío
** Esperado: primera llamada devuelve NULL directamente
*/
static void	test_empty_file(void)
{
	int		fd;
	char	*line;

	printf(CYAN "\n[TEST 3] Archivo vacío\n" RESET);
	create_test_file("/tmp/gnl_t3.txt", "");
	fd = open("/tmp/gnl_t3.txt", O_RDONLY);
	line = get_next_line(fd);
	if (!line)
		printf(GREEN "  OK: devolvió NULL en archivo vacío\n" RESET);
	else
	{
		printf(RED "  FALLO: devolvió algo en archivo vacío: [%s]\n" RESET, line);
		free(line);
	}
	close(fd);
}

/*
** TEST 4 — Una sola línea con \n
** Esperado: devuelve "hola\n", luego NULL
*/
static void	test_single_line(void)
{
	int		fd;
	char	*line;

	printf(CYAN "\n[TEST 4] Una sola línea con \\n\n" RESET);
	create_test_file("/tmp/gnl_t4.txt", "hola mundo\n");
	fd = open("/tmp/gnl_t4.txt", O_RDONLY);
	line = get_next_line(fd);
	print_visible("  1ª llamada", line);
	free(line);
	line = get_next_line(fd);
	print_visible("  2ª llamada", line); /* NULL */
	close(fd);
}

/*
** TEST 5 — Solo un \n (línea completamente vacía)
** Esperado: devuelve "\n", luego NULL
*/
static void	test_only_newline(void)
{
	int		fd;
	char	*line;

	printf(CYAN "\n[TEST 5] Solo un \\n\n" RESET);
	create_test_file("/tmp/gnl_t5.txt", "\n");
	fd = open("/tmp/gnl_t5.txt", O_RDONLY);
	line = get_next_line(fd);
	print_visible("  1ª llamada", line);
	free(line);
	line = get_next_line(fd);
	print_visible("  2ª llamada", line); /* NULL */
	close(fd);
}

/*
** TEST 6 — fd inválido (-1)
** Esperado: NULL desde el principio
*/
static void	test_invalid_fd(void)
{
	char	*line;

	printf(CYAN "\n[TEST 6] fd inválido (-1)\n" RESET);
	line = get_next_line(-1);
	if (!line)
		printf(GREEN "  OK: devolvió NULL con fd=-1\n" RESET);
	else
	{
		printf(RED "  FALLO: no debería devolver nada\n" RESET);
		free(line);
	}
}

/*
** TEST 7 — Línea muy larga (más grande que BUFFER_SIZE)
** Prueba que el stash acumula bien varios reads
** Esperado: devuelve la línea completa aunque BUFFER_SIZE sea pequeño
*/
static void	test_long_line(void)
{
	int		fd;
	char	*line;
	char	buf[1024];
	int		i;

	printf(CYAN "\n[TEST 7] Línea larga (> BUFFER_SIZE)\n" RESET);
	/* Construye una línea de 200 'x' + \n */
	i = 0;
	while (i < 200)
		buf[i++] = 'x';
	buf[i++] = '\n';
	buf[i] = '\0';
	create_test_file("/tmp/gnl_t7.txt", buf);
	fd = open("/tmp/gnl_t7.txt", O_RDONLY);
	line = get_next_line(fd);
	if (line && strlen(line) == 201)
		printf(GREEN "  OK: longitud correcta (%zu chars)\n" RESET, strlen(line));
	else
		printf(RED "  FALLO: longitud %zu (esperado 201)\n" RESET,
			line ? strlen(line) : 0);
	free(line);
	close(fd);
}

/*
** TEST 8 — Muchos \n seguidos (líneas vacías consecutivas)
** Esperado: cada llamada devuelve "\n", luego NULL
*/
static void	test_multiple_empty_lines(void)
{
	int		fd;
	char	*line;
	int		i;

	printf(CYAN "\n[TEST 8] Varias líneas vacías (\\n\\n\\n)\n" RESET);
	create_test_file("/tmp/gnl_t8.txt", "\n\n\n");
	fd = open("/tmp/gnl_t8.txt", O_RDONLY);
	i = 1;
	while (1)
	{
		line = get_next_line(fd);
		if (!line)
			break ;
		printf(GREEN "  Línea %d" RESET, i++);
		print_visible("", line);
		free(line);
	}
	close(fd);
}

/*
** TEST 9 — Dos fds abiertos simultáneamente
** ATENCIÓN: con una sola variable estática, GNL básico NO soporta
** múltiples fds. Este test es para que veas ese comportamiento.
** (El bonus de GNL sí lo soporta, pero tú entregas la versión sin bonus)
*/
static void	test_two_fds(void)
{
	int		fd1;
	int		fd2;
	char	*l1;
	char	*l2;

	printf(CYAN "\n[TEST 9] Dos fds simultáneos (comportamiento indefinido en versión básica)\n" RESET);
	printf(YELLOW "  NOTA: la versión básica con static char *stash NO soporta esto.\n" RESET);
	printf(YELLOW "  El resultado puede ser incorrecto — es esperado.\n" RESET);
	create_test_file("/tmp/gnl_fa.txt", "archivo A linea 1\narchivo A linea 2\n");
	create_test_file("/tmp/gnl_fb.txt", "archivo B linea 1\narchivo B linea 2\n");
	fd1 = open("/tmp/gnl_fa.txt", O_RDONLY);
	fd2 = open("/tmp/gnl_fb.txt", O_RDONLY);
	l1 = get_next_line(fd1);
	print_visible("  fd1 línea 1", l1);
	free(l1);
	l2 = get_next_line(fd2);
	print_visible("  fd2 línea 1", l2);
	free(l2);
	l1 = get_next_line(fd1);
	print_visible("  fd1 línea 2", l1);
	free(l1);
	close(fd1);
	close(fd2);
}

/*
** TEST 10 — Leer desde stdin (manual)
** Descomenta si quieres probarlo a mano
*/
/*
static void	test_stdin(void)
{
	char	*line;

	printf(CYAN "\n[TEST 10] stdin — escribe líneas y pulsa Enter. Ctrl+D para salir.\n" RESET);
	while (1)
	{
		line = get_next_line(0);
		if (!line)
			break ;
		printf(GREEN "  Leído: " RESET);
		print_visible("", line);
		free(line);
	}
}
*/

/* ─── Main ────────────────────────────────────────────────────────────────── */

int	main(void)
{
	printf(YELLOW "══════════════════════════════════════════\n");
	printf("   GNL TEST SUITE  |  BUFFER_SIZE = %d\n", BUFFER_SIZE);
	printf("══════════════════════════════════════════\n" RESET);

	test_normal_file();
	test_no_newline_at_end();
	test_empty_file();
	test_single_line();
	test_only_newline();
	test_invalid_fd();
	test_long_line();
	test_multiple_empty_lines();
	test_two_fds();
	/* test_stdin(); */

	printf(YELLOW "\n══════════════════════════════════════════\n");
	printf("   FIN DE LOS TESTS\n");
	printf("══════════════════════════════════════════\n" RESET);
	return (0);
}