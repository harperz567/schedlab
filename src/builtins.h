#ifndef BUILTINS_H
#define BUILTINS_H

/*
 * Try to run argv as a built-in command
 * (exit, cd, pwd, setconcurrency, setlatency).
 * Returns 1 if argv[0] is a built-in and was handled.
 * Returns 0 if it is not a built-in, so the caller
 * should run it with fork/exec instead.
 */
int run_builtin(char **argv);

#endif
