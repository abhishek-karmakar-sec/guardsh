#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "builtin.h"
#include "guard.h"

#define MAX_LINE_LEN 1024
#define MAX_ARGS 64

static int tokenize_line(char *line, char **argv) {
	int argc = 0;
	char *token = strtok(line, " \t\r\n");

	while (token != NULL && argc < MAX_ARGS - 1) {
		argv[argc++] = token;
		token = strtok(NULL, " \t\r\n");
	}
	argv[argc] = NULL;
	return argc;
}

static void dispatch_external(char **argv) {
	pid_t pid = fork();

	if (pid < 0) {
		perror("guardsh: fork failed");
		return;
	}

	if (pid == 0) {
		/* Child process */
		if (execvp(argv[0], argv) == -1) {
			perror("guardsh");
		}
		exit(EXIT_FAILURE);
	}
	else {
		/* Parent process */
		int status;
		waitpid(pid, &status, 0);
	}
}

int main(void) {
	char line[MAX_LINE_LEN];
	char *argv[MAX_ARGS];

	while (1) {
		printf("\033[1;34mguardsh>\033[0m ");
		fflush(stdout);

		if (fgets(line, sizeof(line), stdin) == NULL) {
			printf("\n");
			break; /* Handle EOF (Ctrl+D) cleanly */
		}

		int argc = tokenize_line(line, argv);
		if (argc == 0) {
			continue; /* Empty line entered. */
		}

		/* Check parent-level built-ins */
		if (is_builtin(argv[0])) {
			execute_builtin(argc, argv);
			continue;
		}

		/* Guard layer verification */
		if (!guard_intercept(&argc, argv)) continue; /*Aborted by user.*/

		/* Dispatch approved external binary */
		dispatch_external(argv);
	}

	return 0;
}
