#include <stdio.h>      // fputs, fgets, stdin, stderr
#include <stdlib.h>     // malloc
#include <string.h>     // strncpy, strlen, strncmp
#include <sys/wait.h>   // waitpid, WUNTRACED
#include <unistd.h>     // fork, execvp

static int execute(char **pieces) {
    pid_t child_pid = fork();
    int status;

    if (child_pid == 0) {
        if (execvp(pieces[0], pieces) == -1) {
            perror("-if");
        }
        exit(1);
    } else {
        waitpid(child_pid, &status, WUNTRACED);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            // is there a more "correct" way to do this?
            return 128 + WTERMSIG(status);
        }
        return 0;
    }
}

static int usage() {
    fputs("Usage: if CONDITION then { CONSEQUENT } else { ALTERNATIVE }", stderr);
    return 1;
}

int main(int argc, char **argv) {
    if (argc < 10) {
        return usage();
    }

    char **condition = argv + 1;
    char **consequent = NULL;
    char **alternative = NULL;

    int in_cond = 1;
    int in_cons = 0;
    int in_altr = 0;
    int done_altr = 0;
    for (int i = 1; i < argc; i++) {
        if (done_altr) {
            return usage();
        }

        if (in_altr) {
            if (strcmp(argv[i], "}") == 0) {
                argv[i] = NULL;
                in_altr = 0;
                done_altr = 1;
            }
        }

        if (in_cons) {
            if (strcmp(argv[i], "}") == 0) {
                argv[i] = NULL;
                alternative = argv + i + 3;
                in_cons = 0;
                in_altr = 1;
            }
        }
        if (in_cond) {
            if (strcmp(argv[i], "then") == 0) {
                argv[i] = NULL;
                consequent = argv + i + 2;
                in_cond = 0;
                in_cons = 1;
            }
        }
    }

    if (condition == NULL) {
        fputs("Condition is null?\n", stderr);
        return 1;
    }
    if (consequent == NULL) {
        fputs("Consequent is null?\n", stderr);
        return 1;
    }
    if (alternative == NULL) {
        fputs("Alternative is null?\n", stderr);
        return 1;
    }

    if (execute(condition) == 0) {
        return execute(consequent);
    } else {
        return execute(alternative);
    }
}
