/**
 * A shell-ish thing for running commands, which is compatible with nothing.
 */

#include <stdio.h>      // fputs, fgets, stdin, stderr
#include <stdlib.h>     // getenv, setenv
#include <string.h>     // strcpy, strlen, strncmp
#include <sys/wait.h>   // waitpid, WUNTRACED
#include <unistd.h>     // fork, execvp

#define LINE_BUF_SIZE (128 * 1024) // A line can be 128KB.
#define SHELLSPLIT_MAX_PIECES 1000 // Number of words per line.

typedef struct Settings_s {
    int quick_exit;
    int quiet;
} Settings;

typedef struct State_s {
    int exit_status;
} State;

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

static char *prompt(char buf[LINE_BUF_SIZE], Settings *settings) {
    if (!settings->quiet) {
        fputs("$ ", stdout);
    }
    return fgets(buf, LINE_BUF_SIZE - 1, stdin);
}

static int execute(ShellSplitResult *result, State *state, Settings *settings) {
    pid_t child_pid = fork();
    int stat_loc;

    if (child_pid == 0) {
        execvp(result->pieces[0], result->pieces);
        fputs("-ish: error: failed to execute command.\n", stderr);
        if (settings->quick_exit) {
            state->exit_status = 1;
            return 1;
        }
    } else {
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }
    return 0;
}

static void handle_env_vars(ShellSplitResult *result, char scratch[LINE_BUF_SIZE]) {
    for (size_t i = 0; i < result->num_pieces; i++) {
        char *tmp = result->pieces[i];
        if (tmp[0] == '$' && tmp[1] == '{' && tmp[strlen(tmp) - 1] == '}') {
            strcpy(scratch, tmp + 2);
            scratch[strlen(scratch) - 1] = '\0';
            result->pieces[i] = getenv(scratch);
        }
    }
}

static int handle_builtins(ShellSplitResult *result, State *state, Settings *settings) {
    (void)settings;
    if (strcmp(result->pieces[0], "exit") == 0) {
        if (result->num_pieces == 1) {
            state->exit_status = 0;
        } else {
            printf("\n\nTODO: Set exit_status=%s\n", result->pieces[1]);
            state->exit_status = 123;
        }
        return 1;
    }

    return 0;
}

static int handle(char buf[LINE_BUF_SIZE], State *state, Settings *settings) {
    static char tmp[LINE_BUF_SIZE] = {0};

    size_t len = strlen(buf);
    if (buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }

    if (strlen(buf) == 0) {
        return 0;
    }

    ShellSplitResult result = {0};
    shellsplit(&result, buf);
    handle_env_vars(&result, tmp);

    if (handle_builtins(&result, state, settings)) {
        return 1;
    }

    if (execute(&result, state, settings)) {
        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {
    char buf[LINE_BUF_SIZE] = {0};
    Settings settings = {0};
    State state = {0};

    int help = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') { continue; }
        for (size_t j = 1; j < strlen(argv[i]); j++) {
            if (argv[i][j] == 'q') { settings.quiet = 1; }
            if (argv[i][j] == 'x') { settings.quick_exit = 1; }
            if (argv[i][j] == 'h') { help = 1; break; }
        }
    }
    if (help) {
        puts("Usage: ish [-q] [-x] [-h]");
        puts("-q    Quiet");
        puts("-x    Exit immediately on error");
        puts("-h    Print help text and exit");
        return 1;
    }

    while (1) {
        char *tmp = prompt(buf, &settings);
        if (tmp == NULL) { break; }
        int ret = handle(buf, &state, &settings);
        if (ret) { break; }
    }

    return state.exit_status;
}
