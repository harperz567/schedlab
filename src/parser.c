#include <string.h>
#include "parser.h"

static int is_redirect(const char *tok)
{
    return strcmp(tok, ">") == 0 || strcmp(tok, "<") == 0 ||
           strcmp(tok, ">>") == 0 || strcmp(tok, "2>") == 0;
}

int parse_line(char *line, command_t *cmd)
{
    memset(cmd, 0, sizeof(*cmd));

    char *tok = strtok(line, " \t");
    while (tok != NULL) {
        if (strcmp(tok, "|") == 0) {
            cmd->has_pipe = 1;
        } else if (is_redirect(tok)) {
            cmd->has_redirect = 1;
        } else if (strcmp(tok, "&") == 0) {
            cmd->background = 1;
        } else {
            if (cmd->argc >= MAX_ARGS)
                return -1;
            cmd->argv[cmd->argc++] = tok;
        }
        tok = strtok(NULL, " \t");
    }

    cmd->argv[cmd->argc] = NULL;
    return 0;
}
