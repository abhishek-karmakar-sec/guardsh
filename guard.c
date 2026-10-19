#include "guard.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static int strip_flag_y(int *argc, char **argv) {
	int bypass = 0;
	int i = 1;

	while (i < *argc) {
		if (strcmp(argv[i], "-y") == 0) {
			bypass = 1;
			/* Shift all subsequent pointers left by one */
			for (int j = i;j < *argc; j++) {
				argv[j] = argv[j + 1];
			}
			(*argc)--;
			/* Re-check same index in case multiple '-y' flags were passed */
		}
		else i++;
	}
	return bypass;
}

static int ask_confirmation(const char *reason, const char *target_summary) {
	printf("\n\033[1;33m[guardsh safety warning]\033[0m: %s\n", reason);
	if (target_summary && strlen(target_summary) > 0) {
		printf("Target: \033[1;37m%s\033[0m\n", target_summary);
	}
	printf("Do you want to proceed? [y/N]: ");
	fflush(stdout);

	char response[16];
	if (fgets(response, sizeof(response), stdin) == NULL) {
		return 0;
	}

	if (response[0] == 'y' || response[0] == 'Y') {
		return 1;
	}

	printf("\033[1;31mOperation cancelled by user.\033[0m\n\n");
	return 0;
}

int guard_intercept(int *argc, char **argv) {
	if (*argc == 0 || argv[0] == NULL) {
		return 1;
	}

	int bypass = strip_flag_y(argc, argv);
	if (bypass) {
		return 1; /* Bypass confirmed so executing the operation after removing the '-y' flag. */
	}

	/* Guard 1: rm (Unlinking files/directories) */
	if (strcmp(argv[0], "rm") == 0) {
		if (*argc < 2) return 1; /* Let rm report missing operand. */
		return ask_confirmation(
				"Permanent file deletion requested (bypasses Trash).",
				argv[1]
				);
	}

	/* Guard 2: mv (Check if destination file already exists to prevent over-write) */
	if (strcmp(argv[0], "mv") == 0) {
		if (*argc >= 3) {
			char *dest = argv[*argc - 1];
			if (access(dest, F_OK) == 0) {
				return ask_confirmation(
						"Destination already exists; file will be overwritten.",
						dest
						);
			}
		}
		return 1;
	}

	/* Guard 3: dd (Raw block level writes) */
	if (strcmp(argv[0], "dd") == 0) {
		return ask_confirmation(
				"Direct block-level disk write initiated.",
				"Raw disk sector modification risk"
				);
	}

	return 1;
}
