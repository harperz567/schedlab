#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "exec.h"

int run_command(command_t *cmd)
{
    /* Stubs: these will be implemented in later sprints */
    if (cmd->has_pipe) {
        printf("schedlab: pipes are not supported yet\n");
        return -1;
    }
    if (cmd->has_redirect) {
        printf("schedlab: redirects are not supported yet\n");
        return -1;
    }
    if (cmd->background) {
        printf("schedlab: background jobs are not supported yet\n");
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("schedlab: fork");
        return -1;
    }

    if (pid == 0) {
        /* Child: replace this process with the command */
        execvp(cmd->argv[0], cmd->argv);

        /* Only reached if execvp fails */
        if (errno == ENOENT)
            fprintf(stderr, "schedlab: %s: command not found\n",
                    cmd->argv[0]);
        else
            fprintf(stderr, "schedlab: %s: %s\n",
                    cmd->argv[0], strerror(errno));
        _exit(127);
    }

    /* Parent: wait for the child, retrying if interrupted */
    int status;
    while (waitpid(pid, &status, 0) < 0) {
        if (errno != EINTR) {
            perror("schedlab: waitpid");
            return -1;
        }
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return -1;
}
