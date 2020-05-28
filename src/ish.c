/**
 * A shell-ish thing for running commands, which is compatible with nothing.
 */

#include <stdio.h>      // fputs, fgets, stdin, stderr
#include <stdlib.h>     // getenv, setenv
#include <string.h>     // strncpy, strlen, strncmp
#include <sys/wait.h>   // waitpid, WUNTRACED
#include <unistd.h>     // fork, execvp

#define LINE_BUF_SIZE (128 * 1024) // A line can be 128KB.
#define SHELLSPLIT_MAX_PIECES 1000 // Number of words per line.

typedef struct Settings_s {
    int quick_exit;
    int quiet;
} Settings;

typedef struct ShellSplitResult_s {
    char *pieces[SHELLSPLIT_MAX_PIECES];
    size_t num_pieces;
} ShellSplitResult;

static size_t decimal_places_in_int(int n) {
    size_t decimal_places = 0;
    int tmp = n;

    while (tmp >= 1) {
        decimal_places += 1;
        tmp /= 10;
    }

    if (decimal_places == 0) {
        decimal_places = 1;
    }

    return decimal_places;
}

#define INT_BUF_SIZE (sizeof(char) * (sizeof(int) + 2)) // TODO: Verify this.
// Hard-coded to be base 10 for simplicity.
static char *int_to_str(char result[INT_BUF_SIZE], int n) {
    char buf[INT_BUF_SIZE] = {0};
    int tmp = n;

    size_t decimal_places = decimal_places_in_int(n);

    for (size_t idx = 0; idx < decimal_places; idx++) {
        buf[idx] = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp % 10];
        tmp /= 10;
    }

    result[decimal_places] = 0;

    for (size_t i = 0; i < decimal_places; i++) {
        result[i] = buf[decimal_places - i - 1];
    }
    result[decimal_places] = 0;

    return result;
}

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

static int handle_builtins(ShellSplitResult *result);
static int execute(ShellSplitResult *result) {
    if (handle_builtins(result)) {
        return 0;
    }

    pid_t child_pid = fork();
    int status;

    if (child_pid == 0) {
        if (execvp(result->pieces[0], result->pieces) == -1) {
            perror("-ish");
        }
        exit(1);
    } else {
        waitpid(child_pid, &status, WUNTRACED);

        int ret = 0;
        if (WIFEXITED(status)) {
            ret = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            // is there a more "correct" way to do this?
            ret = 128 + WTERMSIG(status);
        }

        return 0;
    }
}

static void handle_env_vars(ShellSplitResult *result, char scratch[LINE_BUF_SIZE]) {
    for (size_t i = 0; i < result->num_pieces; i++) {
        char *tmp = result->pieces[i];
        if (tmp[0] == '$' && tmp[1] == '{' && tmp[strlen(tmp) - 1] == '}') {
            strncpy(scratch, tmp + 2, LINE_BUF_SIZE);
            scratch[strlen(scratch) - 1] = '\0';
            result->pieces[i] = getenv(scratch);
        }
    }
}

static int handle_builtins(ShellSplitResult *result) {
    if (strncmp(result->pieces[0], "exit", 5) == 0) {
        if (result->num_pieces == 1) {
            exit(0);
        } else {
            printf("\n\nTODO: Set exit status=%s\n", result->pieces[1]);
            exit(123);
        }
    }

    return 0;
}

static void handle(char buf[LINE_BUF_SIZE], Settings *settings) {
    (void)settings;
    static char intbuf[INT_BUF_SIZE] = {0};
    static char tmp[LINE_BUF_SIZE] = {0};

    size_t len = strlen(buf);
    if (buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }

    if (strlen(buf) == 0) {
        return;
    }

    ShellSplitResult result = {0};
    shellsplit(&result, buf);
    handle_env_vars(&result, tmp);

    int status = execute(&result);
    setenv("?", int_to_str(intbuf, status), 1);
}

int main(int argc, char **argv) {
    char buf[LINE_BUF_SIZE] = {0};
    Settings settings = {0};

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

    while (prompt(buf, &settings) != NULL) {
        handle(buf, &settings);
    }

    return 0;
}
