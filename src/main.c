#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtins.h"
#include "parser.h"
#include "exec.h"

#define PROMPT "schedlab> "

int main(void)
{
    char *line = NULL;
    size_t cap = 0;
    command_t cmd;

    printf("SchedLab shell (skeleton)\n");

    while (1) {
        fputs(PROMPT, stdout);
        fflush(stdout);

        ssize_t len = getline(&line, &cap, stdin);
        if (len == -1) {
            /* EOF (Ctrl+D) or read error: leave the shell */
            printf("\n");
            break;
        }

        /* Strip the trailing newline */
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        if (parse_line(line, &cmd) < 0) {
            fprintf(stderr, "schedlab: too many arguments\n");
            continue;
        }

        /* Skip empty lines */
        if (cmd.argc == 0)
            continue;

        /* Built-ins run inside the shell process */
        if (run_builtin(cmd.argv))
            continue;

        /* Everything else runs in a child process */
        run_command(&cmd);
    }

    free(line);
    return 0;
}
