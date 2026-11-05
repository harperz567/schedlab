#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "builtins.h"
#include "parser.h"

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

        /* Temporary: show the parse result */
        for (int i = 0; i < cmd.argc; i++)
            printf("argv[%d] = %s\n", i, cmd.argv[i]);
        printf("pipe=%d redirect=%d background=%d\n",
               cmd.has_pipe, cmd.has_redirect, cmd.background);
    }

    free(line);
    return 0;
}
