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

static struct Settings_s {
    int no_prompt;
    int quick_exit;
} settings = {0};

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

static size_t shellsplit(char **pieces, char input[LINE_BUF_SIZE]) {
    int in_dq_str = 0;
    int in_sq_str = 0;

    char *tmp = input;
    while (tmp[0] == ' ') {
        tmp++;
    }

    size_t num_pieces = 1;
    pieces[0] = tmp;

    for (; tmp < (input + LINE_BUF_SIZE) && *tmp; tmp++) {
        switch (*tmp) {
        case '"':
            if (!in_sq_str) {
                if (in_dq_str == 0) {
                    in_dq_str = 1;
                    num_pieces++;
                    pieces[num_pieces - 1] = tmp + 1;
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
                    num_pieces++;
                    pieces[num_pieces - 1] = tmp + 1;
                } else {
                    in_sq_str = 0;
                }
                *tmp = '\0';
            }
            break;
        case ' ':
            if (in_sq_str == 0 && in_dq_str == 0) {
                if (*(tmp + 1) != ' ' &&
                    *(tmp + 1) != '\'' && *(tmp + 1) != '"') {
                    num_pieces++;
                    pieces[num_pieces - 1] = tmp + 1;
                }
                *tmp = '\0';
            }
            break;
        }
    }
    return num_pieces;
}

static char *prompt(char buf[LINE_BUF_SIZE]) {
    if (!settings.no_prompt) {
        fputs("$ ", stdout);
    }
    return fgets(buf, LINE_BUF_SIZE, stdin);
}

static void fail(char *msg) {
    fputs(msg, stderr);
    setenv("?", "1", 1);
}

static void print_if_usage() {
    fail("Usage: if CONDITION then { CONSEQUENT } else { ALTERNATIVE }\n");
}

static int handle_builtins(size_t argc, char **argv);
static int execute(size_t argc, char **argv) {
    if (handle_builtins(argc, argv)) {
        return 0;
    }

    pid_t child_pid = fork();
    int status;

    if (child_pid == 0) {
        if (execvp(argv[0], argv) == -1) {
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
        if (settings.quick_exit && ret != 0) {
            exit(ret);
        }
        return ret;
    }
}

static void handle_env_vars(size_t argc, char **argv, char scratch[LINE_BUF_SIZE]) {
    for (size_t i = 0; i < argc; i++) {
        char *tmp = argv[i];
        if (tmp[0] == '$' && tmp[1] == '{' && tmp[strlen(tmp) - 1] == '}') {
            strncpy(scratch, tmp + 2, LINE_BUF_SIZE);
            scratch[strlen(scratch) - 1] = '\0';
            argv[i] = getenv(scratch);
        }
    }
}

static int handle_if(size_t argc, char **argv) {
    char **condition = argv + 1;
    char **consequent = NULL;
    char **alternative = NULL;

    int in_cond = 1;
    int in_cons = 0;
    int in_altr = 0;
    size_t cond_argc = 0;
    size_t cons_argc = 0;
    size_t altr_argc = 0;
    for (size_t i = 1; i < argc; i++) {
        if (in_altr) {
            if (strcmp(argv[i], "}") == 0) {
                argv[i] = NULL;
                in_altr = 0;
                if (argc > (i + 1)) {
                    print_if_usage();
                    return 1;
                }
            } else {
                altr_argc++;
            }
        }
        if (in_cons) {
            if (strcmp(argv[i], "}") == 0) {
                argv[i] = NULL;
                alternative = argv + i + 3;
                in_cons = 0;
                in_altr = 1;
            } else {
                cons_argc++;
            }
        }
        if (in_cond) {
            if (strcmp(argv[i], "then") == 0) {
                argv[i] = NULL;
                consequent = argv + i + 2;
                in_cond = 0;
                in_cons = 1;
            } else {
                cond_argc++;
            }
        }
    }

    if (condition == NULL || consequent == NULL || alternative == NULL) {
        print_if_usage();
        return 1; // In theory, this means it got invalid arguments.
    }

    if (execute(cond_argc, condition) == 0) {
        return execute(cons_argc, consequent);
    } else {
        return execute(altr_argc, alternative);
    }
}

static int handle_builtins(size_t argc, char **argv) {
    if (strncmp(argv[0], "exit", 5) == 0) {
        if (argc == 1) {
            exit(0);
        } else {
            printf("\n\nTODO: Set exit status=%s\n", argv[1]);
            exit(123);
        }
    } else if (strncmp(argv[0], "if", 3) == 0) {
        if (argc >= 10) {
            handle_if(argc, argv);
        } else {
            print_if_usage();
        }
        return 1; // handled by a builtin
    }
    return 0; // not handled a builtin
}

static void handle(char buf[LINE_BUF_SIZE]) {
    static char intbuf[INT_BUF_SIZE] = {0};
    static char tmp[LINE_BUF_SIZE] = {0};

    size_t len = strlen(buf);
    if (len == 0) {
        return;
    }

    if (buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }

    if (strlen(buf) == 0) {
        return;
    }

    char *argv[SHELLSPLIT_MAX_PIECES + 1] = {0};
    size_t argc = shellsplit(argv, buf);
    handle_env_vars(argc, argv, tmp);

    int status = execute(argc, argv);
    setenv("?", int_to_str(intbuf, status), 1);
}

int main(int argc, char **argv) {
    char buf[LINE_BUF_SIZE] = {0};

    int help = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') { continue; }
        for (size_t j = 1; j < strlen(argv[i]); j++) {
            if (argv[i][j] == 'q') { settings.no_prompt = 1; }
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

    while (prompt(buf) != NULL) {
        handle(buf);
    }

    return 0;
}
