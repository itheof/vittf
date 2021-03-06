#ifndef VITTF_H
#define VITTF_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

/*
 * Colors
 */
#define COLOR_RESET	"\x1b[0m"
#define BG_RED		"\x1b[41m"
#define FG_BLUE		"\x1b[34m"
#define FG_GREEN	"\x1b[32m"

/*
 * Strings
 */
#define FAILED_STR		printf(BG_RED "FAILED !!!\t" COLOR_RESET)
#define FAILED_FILE		printf("FILE(%s)", __FILE__)
#define FAILED_LINE		printf(" | LINE(%d)", __LINE__)
#define FAILED_FUNCTION	printf(" | FUNCTION(%s)", __func__)
#define PRINTF_FAILED	FAILED_STR; FAILED_FILE; FAILED_LINE; FAILED_FUNCTION;

/*
 * Assertions
 */
#define v_assert_type(fmt, expected, op, actual) \
	do { \
		if (!(expected op actual)) { \
			PRINTF_FAILED; \
			printf("\n\tEXPRESSION\t>>> " #expected " " #op " " #actual); \
			printf("\n\tVALUES\t\t>>> " fmt " " #op " " fmt, (expected), (actual)); \
			printf("\n"); \
			exit(1); \
		} \
	} while (0)

#define v_assert_type_pass(fmt, expected, op, actual) \
	do { \
		if (!(expected op actual)) { \
			PRINTF_FAILED; \
			printf("\n\tEXPRESSION\t>>> " #expected " " #op " " #actual); \
			printf("\n\tVALUES\t\t>>> " fmt " " #op " " fmt, (expected), (actual)); \
			printf("\n"); \
			return ; \
		} \
	} while (0)

#define v_assert_int(expected, op, actual) \
	v_assert_type("%d", (int)expected, op, (int)actual)
#define v_assert_uint(expected, op, actual) \
	v_assert_type("%u", (unsigned)(expected), op, (unsigned)(actual))
#define v_assert_long(expected, op, actual) \
	v_assert_type("%ld", (long)(expected), op, (long)(actual))
#define v_assert_size_t(expected, op, actual) \
	v_assert_type("%zu", (size_t)(expected), op, (size_t)(actual))
#define v_assert_ptr(expected, op, actual) \
	v_assert_type("%p", (void*)(expected), op, (void*)(actual))
#define v_assert_char(expected, op, actual) \
	v_assert_type("%c", (char)expected, op, (char)actual)
#define v_assert_uintptr(expected, op, actual) \
	v_assert_type("%tx", (uintptr_t)expected, op, (uintptr_t)actual)

#define v_assert_int_pass(expected, op, actual) \
	v_assert_type_pass("%d", (int)expected, op, (int)actual)
#define v_assert_uint_pass(expected, op, actual) \
	v_assert_type_pass("%u", (unsigned)(expected), op, (unsigned)(actual))
#define v_assert_long_pass(expected, op, actual) \
	v_assert_type_pass("%ld", (long)(expected), op, (long)(actual))
#define v_assert_size_t_pass(expected, op, actual) \
	v_assert_type_pass("%zu", (size_t)(expected), op, (size_t)(actual))
#define v_assert_ptr_pass(expected, op, actual) \
	v_assert_type_pass("%p", (void*)(expected), op, (void*)(actual))
#define v_assert_char_pass(expected, op, actual) \
	v_assert_type_pass("%c", (char)expected, op, (char)actual)
#define v_assert_uintptr_pass(expected, op, actual) \
	v_assert_type_pass("%tx", (uintptr_t)expected, op, (uintptr_t)actual)

#define v_assert(expression) \
	do { \
		if (!(expression)) {\
			PRINTF_FAILED; \
			printf("\n\tExpression(%s)\n", #expression); \
			exit(1); \
		} \
	} while (0)

#define v_assert_str(expected, actual) \
	do { \
		if (strcmp((expected), (actual)) != 0) {\
			PRINTF_FAILED; \
			printf("\n\tStrcmp >>>\n\tExpected\t--> [%s]\n\tActual\t\t--> [%s]\n", (char*)(expected), (char*)(actual)); \
			exit(1);\
		}\
	} while (0)

#define v_assert_pass(expression) \
	do { \
		if (!(expression)) {\
			PRINTF_FAILED; \
			printf("\n\tExpression(%s)\n", #expression); \
			return ;\
		} \
	} while (0)

#define v_assert_str_pass(expected, actual) \
	do { \
		if (strcmp((expected), (actual)) != 0) {\
			PRINTF_FAILED; \
			printf("\n\tExpression >>> (Expected) %s != %s (Actual)\n", (char*)(expected), (char*)(actual)); \
			return ;\
		}\
	} while (0)

#define v_test_success(name) \
	(printf("%s ✓\n", (name)))
#define v_suite_success(suite) \
	(printf("---> Suite %s ✓\n\n", (suite)))
#define v_full_success(test) \
	(printf("=========\n\x1b[32mFULL TEST FOR %s ✓\x1b[0m\n", (test)))
#define VTS v_test_success(__func__)
#define VSS v_suite_success(__func__)

/*
 * Stdout Redirection
 *
 * - Créer une référence supplémentaire vers le FILE de stdout
 *		Les objets pointés par les référence ne sont pas distinguable
 *
 * [0] => connect to the read end of the pipe (output)
 * [1] => connect to the write end of the pipe (input)
 * Les données écrite dans [1] peuvent etre lues dans [0]
 * Le pipe est actif tant que les 2 fds ne sont pas clos
 *
 * Le fd 1 étant déjà occupé, dup2() le clos.
 * Ensuite dup2() duplique la référence vers l'input du pipe en utilisant
 * le second parametre envoyé, a savoir 1
 */
static int	v_stdout_ref;
static int	v_pipe_redirect[2];
static int	v_read_return;

#define V_REDIRECT_STDOUT_SETUP \
	do { \
		setvbuf(stdout, NULL, _IONBF, BUFSIZ); \
		v_stdout_ref = dup(1); \
		if ((pipe(v_pipe_redirect)) == -1) {\
			fprintf(stderr, "Pipe() error !\n"); \
			exit(10); \
		}\
		if ((dup2(v_pipe_redirect[1], 1)) == -1) {\
			fprintf(stderr, "Dup2() error !\n"); \
			exit(11); \
		}\
	} while (0)

#define V_REDIRECT_STDOUT_READ(v_buffer, v_buffer_size) \
	do { \
		v_read_return = read(v_pipe_redirect[0], v_buffer, v_buffer_size - 1); \
		if (v_read_return == -1) {\
			fprintf(stderr, "Read() error !\n"); \
			exit(12); \
		}\
		v_buffer[v_read_return] = '\0';\
	} while (0)

#define V_REDIRECT_STDOUT_TEARDOWN \
	do { \
		if ((dup2(v_stdout_ref, 1)) == -1) {\
			fprintf(stderr, "Dup2() error !\n"); \
			exit(13); \
		}\
		close(v_pipe_redirect[0]); \
		close(v_pipe_redirect[1]); \
		close(v_stdout_ref); \
		setlinebuf(stdout); \
	} while (0)

/*
 * Stderr Redirection
 *
 * - Identique a la redirection de stdout ci-dessus mais pour stderr
 *		Utilise la meme variable 'v_read_return' que la redirection de stdout
 *
 * L'affichage des erreurs a ete supprime afin de ne pas fausser les tests.
 *
 */

static int	v_stderr_ref;
static int	v_stderr_pipe[2];

#define V_REDIRECT_STDERR_SETUP \
	do { \
		setvbuf(stderr, NULL, _IONBF, BUFSIZ); \
		v_stderr_ref = dup(2); \
		if ((pipe(v_stderr_pipe)) == -1) {\
			exit(10); \
		}\
		if ((dup2(v_stderr_pipe[1], 2)) == -1) {\
			exit(11); \
		}\
	} while (0)

#define V_REDIRECT_STDERR_READ(v_buffer, v_buffer_size) \
	do { \
		v_read_return = read(v_stderr_pipe[0], v_buffer, v_buffer_size - 1); \
		if (v_read_return == -1) {\
			exit(12); \
		}\
		v_buffer[v_read_return] = '\0';\
	} while (0)

#define V_REDIRECT_STDERR_TEARDOWN \
	do { \
		if ((dup2(v_stderr_ref, 2)) == -1) {\
			exit(13); \
		}\
		close(v_stderr_pipe[0]); \
		close(v_stderr_pipe[1]); \
		close(v_stderr_ref); \
		setlinebuf(stderr); \
	} while (0)

#endif
