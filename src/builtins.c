#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "builtins.h"

/*
 * TODO: implement exit, cd, pwd, and stubs for
 * setconcurrency and setlatency.
 * For now, nothing is treated as a built-in.
*/


int run_builtin(char **argv)
{
    if (argv == NULL || argv[0] == NULL)
        return 0;

    if (strcmp(argv[0], "exit") == 0) {
        exit(0);
    }

    if (strcmp(argv[0], "cd") == 0) {
        const char *path = argv[1];

        if (path == NULL)
            path = getenv("HOME");

        if (path == NULL) {
            fprintf(stderr, "HOME not set\n");
        } else if (chdir(path) != 0) {
            perror("schedlab: cd");
        }

        return 1;
    }

    if (strcmp(argv[0], "pwd") == 0) {
        char *path = getcwd(NULL, 0);

        if (path == NULL) {
            perror("schedlab: pwd");
        } else {
            printf("%s\n", path);
            free(path);
        }

        return 1;
    }

    if (strcmp(argv[0], "setconcurrency") == 0) {
        printf("setconcurrency not implemented yet\n");
        return 1;
    }

    if (strcmp(argv[0], "setlatency") == 0) {
        printf("setlatency not implemented yet\n");
        return 1;
    }

    return 0;
}
