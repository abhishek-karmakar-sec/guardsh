#ifndef GUARD_H
#define GUARD_H

/*
 * Inspects argv for dangerous commands (rm. mv, dd).
 * Strips '-y' in-place if present and updates *argc.
 * Prompts user if dangerous and not bypassed.
 * Returns 1 if execution is allowed, 0 to abort.
 */
int guard_intercept(int *argc, char **argv);

#endif /* GUARD_H */
