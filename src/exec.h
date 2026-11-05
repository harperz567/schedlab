#ifndef EXEC_H
#define EXEC_H

#include "parser.h"

/*
 * Run an external command with fork/exec and wait for it.
 * Pipes, redirects, and background jobs are stubbed for now.
 * Returns the child's exit status, or -1 on error.
 */
int run_command(command_t *cmd);

#endif
