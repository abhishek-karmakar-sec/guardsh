#include "builtin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int is_builtin(const char *cmd) {
	if (cmd == NULL) return 0;
	return (strcmp(cmd, "cd") == 0 ||
		strcmp(cmd, "exit") == 0 ||
		strcmp(cmd, "help") == 0);
}

int execute_builtin(int argc, char **argv) {
	if (strcmp(argv[0], "exit") == 0) {
		printf("Exiting guardsh.\n");
		exit(0);
	}

	if (strcmp(argv[0], "cd") == 0) {
		char *target = argv[1];
		if (target == NULL) {
			target = getenv("HOME");
			if (target == NULL) {
				target = "/";
			}
		}
		if (chdir(target) != 0) {
			perror("guardsh: cd");
		}
		return 1;
	}

	if (strcmp(argv[0], "help") == 0) {
		printf("\n\033[1;32mguardsh - Beginner's Defensive Unix Shell\033[0m\n");
		printf("Built-in commands: cd, exit, help\n\n");
		printf("Safety Features:\n");
		printf("  - Intercepts destructive commands: rm, mv (overwrites), dd\n");
		printf("  - Requires interactive confirmation unless '-y' flag is supplied\n");
		printf("  - Automatically strips '-y' before invoking native binaries\n\n");
		(void)argc;
		return 1;
	}

	return 0;
}
