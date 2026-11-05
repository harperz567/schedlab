#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 64

/* One parsed command line */
typedef struct {
    char *argv[MAX_ARGS + 1]; /* NULL-terminated, ready for execvp */
    int argc;
    int has_pipe;      /* line contains | */
    int has_redirect;  /* line contains >, <, >>, or 2> */
    int background;    /* line ends with & */
} command_t;

/*
 * Split line into tokens separated by spaces or tabs.
 * Modifies line in place; argv entries point into it.
 * Returns 0 on success, -1 if there are too many arguments.
 */
int parse_line(char *line, command_t *cmd);

#endif
