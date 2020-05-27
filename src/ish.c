/**
 * A shell-ish thing for running commands, which is compatible with nothing.
 */

#include <stdio.h>      // fputs, fgets, stdin, stderr
#include <string.h>     // strlen, strncmp
#include <sys/wait.h>   // waitpid, WUNTRACED
#include <unistd.h>     // fork, execvp

#define LINE_BUF_SIZE 4096
#define SHELLSPLIT_MAX_PIECES 64

typedef struct ShellSplitResult_s {
    char *pieces[SHELLSPLIT_MAX_PIECES];
    size_t num_pieces;
} ShellSplitResult;

static void shellsplit(ShellSplitResult *result, char input[LINE_BUF_SIZE]) {
    int in_dq_str = 0;
    int in_sq_str = 0;

    result->num_pieces = 1;
    result->pieces[0] = input;
    for (char *tmp = input; tmp < (input + LINE_BUF_SIZE); tmp++) {
        switch (*tmp) {
        case '"':
            if (!in_sq_str) {
                if (in_dq_str == 0) {
                    in_dq_str = 1;
                    result->num_pieces++;
                    result->pieces[result->num_pieces - 1] = tmp + 1;
                } else {
                    in_dq_str = 0;
                }
                *tmp = '\0';
            }
            break;
        case '\'':
            if (!in_dq_str) {
                if (in_sq_str == 0) {
                    in_sq_str = 1;
                    result->num_pieces++;
                    result->pieces[result->num_pieces - 1] = tmp + 1;
                } else {
                    in_sq_str = 0;
                }
                *tmp = '\0';
            }
            break;
        case ' ':
            if (in_sq_str == 0 && in_dq_str == 0 && *(tmp + 1) != ' ' &&
                    *(tmp + 1) != '\'' && *(tmp + 1) != '"') {
                result->num_pieces++;
                result->pieces[result->num_pieces - 1] = tmp + 1;
                *tmp = '\0';
            }
            break;
        }
    }
}

static char *prompt(char buf[LINE_BUF_SIZE]) {
    fputs("$ ", stdout);
    return fgets(buf, LINE_BUF_SIZE - 1, stdin);
}

static void execute(ShellSplitResult *result) {
    pid_t child_pid = fork();
    int stat_loc;

    if (child_pid == 0) {
        execvp(result->pieces[0], result->pieces);
        fputs("-ish: error: failed to execute command.\n", stderr);
    } else {
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }
}

static int handle(char buf[LINE_BUF_SIZE]) {
    size_t len = strlen(buf);
    if (buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }

    ShellSplitResult result = {0};
    shellsplit(&result, buf);

    if (strncmp(result.pieces[0], "exit", 5) == 0) {
        return 1;
    }

    execute(&result);
    return 0;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    char buf[LINE_BUF_SIZE] = {0};
    int should_exit = 0;

    while (prompt(buf) && !should_exit) {
        should_exit = handle(buf);
    }
}
