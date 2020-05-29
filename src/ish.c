/**
 * A shell-ish thing for running commands, which is compatible with nothing.
 */

#include <stdio.h>      // fputs, fgets, perror, stdout, stderr
#include <stdlib.h>     // exit, getenv, setenv
#include <string.h>     // strlen, strncmp
#include <sys/wait.h>   // waitpid, WEXITSTATUS, WIFEXITED, WIFSIGNALED, WTERMSIG, WUNTRACED
#include <unistd.h>     // fork, execvp

#define VERSION "0.0.1"
#define INT_BUF_SIZE 22 // 20 (max digits in int64) + 1 (sign) + 1 (null)
#define CHARS_PER_LINE (128 * 1024)         // Max chars per line of input
// Can't have more chunks than repeating "X " until buffer is full.
#define PARTS_PER_LINE (CHARS_PER_LINE / 2) // Max words per line.

// `settings` variable holds all the settings.
static struct Settings_s {
    int no_prompt;
    int quick_exit;
} settings = {0};

// Convert an int to a char*, with fixed-size buffers.
static char *int_to_str(char result[INT_BUF_SIZE], int n) {
    char buf[INT_BUF_SIZE] = {0};
    int tmp = n;
    size_t decimal_places = 0;
    do {
        buf[decimal_places] = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp % 10];
        tmp /= 10;
        decimal_places++;
    } while (tmp >= 1);
    buf[decimal_places] = 0;

    for (size_t i = 0; i < decimal_places; i++) {
        result[i] = buf[decimal_places - i - 1];
    }
    result[decimal_places] = 0;

    return result;
}

// Destructively split a line of text in a vaguely-shell-like manner.
static size_t shellsplit(char **pieces, char input[CHARS_PER_LINE]) {
    int in_squote = 0; // To track if we're in a single-quoted string.
    int in_dquote = 0; // To track if we're in a double-quoted string.
    char buf[CHARS_PER_LINE] = {0}; // Temporary buffer.
    size_t input_idx = 0;
    size_t buf_idx = 0;
    size_t num_pieces = 1;
    while (input[input_idx] == ' ') { // Eat leading spaces.
        input_idx++;
        buf_idx++;
    }
    pieces[0] = input + buf_idx;
    for (; input_idx < strlen(input); input_idx++) {
        int is_dquote = (input[input_idx] == '"');
        int is_squote = (input[input_idx] == '\'');
        int is_space = (input[input_idx] == ' ');
        int new_token = (!in_dquote && !in_squote && is_space);
        int consume = !(is_dquote || is_squote || is_space) ||
            (in_dquote && !is_dquote) || (in_squote && !is_squote);

        if (is_dquote && !in_squote) { in_dquote = !in_dquote; }
        if (is_squote && !in_dquote) { in_squote = !in_squote; }
        if (consume)   { buf[buf_idx++] = input[input_idx]; }
        if (new_token) {
            while (input[input_idx + 1] == ' ') { // Eat extra spaces.
                input_idx++;
                buf_idx++;
            }
            num_pieces++;
            pieces[num_pieces - 1] = input + buf_idx + 1;
            buf[buf_idx] = '\0';
            buf_idx++;
        }
    }
    buf[buf_idx] = '\0';
    memcpy(input, buf, buf_idx + 1);
    memset(buf, 1, buf_idx + 1); // To make memory problems more obvious.
    return num_pieces;
}

// Print the prompt (unless settings.no_prompt), return next line of input
static char *prompt(char buf[CHARS_PER_LINE]) {
    if (!settings.no_prompt) {
        fputs("$ ", stdout);
    }
    return fgets(buf, CHARS_PER_LINE, stdin);
}

// Print msg to stderr and set $? to 1.
static void fail(char *msg) {
    fputs(msg, stderr);
    setenv("?", "1", 1);
}
static void print_if_usage() {
    fail("Usage: if CONDITION then { CONSEQUENT } else { ALTERNATIVE }\n");
}

static int handle_builtins(size_t argc, char **argv);
// Attempt to execute a command and return the status code.
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

// If whole word is ${X}, replace with the value of the env variable X.
static void handle_env_vars(size_t argc, char **argv, char scratch[CHARS_PER_LINE]) {
    for (size_t i = 0; i < argc; i++) {
        char *tmp = argv[i];
        if (tmp[0] == '$' && tmp[1] == '{' && tmp[strlen(tmp) - 1] == '}') {
            strncpy(scratch, tmp + 2, CHARS_PER_LINE);
            scratch[strlen(scratch) - 1] = '\0';
            argv[i] = getenv(scratch);
        }
    }
}

// Handle executing if/else statements.
// Format is `if CONDITION then { CONSEQUENT } else { ALTERNATIVE }`.
static int handle_if(size_t argc, char **argv) {
    char **condition = argv + 1; // Mark start of command in the condition
    char **consequent = NULL;
    char **alternative = NULL;
    int in_cond = 1; // First word is `if`; we know we're in the condition
    int in_cons = 0; // 1 = we're parsing the `consequent` branch.
    int in_altr = 0; // 1 = we're parsing the `alternative` branch.
    size_t cond_argc = 0;
    size_t cons_argc = 0;
    size_t altr_argc = 0;
    for (size_t i = 1; i < argc; i++) { // Split `if` line into 3 commands
        if (in_altr) { // If we're after the `else`
            if (strncmp(argv[i], "}", 2) == 0) { // Found closing '}'!
                argv[i] = NULL; // Mark end of `char **alternative`.
                in_altr = 0; // Indicate we're done with `alternative`.
                if (argc > (i + 1)) { // If we have too many args...
                    print_if_usage(); // ... print usage ...
                    return 1; // ... and then bail.
                }
            } else { // No closing '}'; keep going.
                altr_argc++;
            }
        }
        if (in_cons) { // If we're after the `then` but before the `else`.
            if (strncmp(argv[i], "}", 2) == 0) { // Found closing '}'!
                argv[i] = NULL; // Mark end of `char **consequent`.
                alternative = argv + i + 3; // Mark start of `alternative`
                in_cons = 0; // Indicate we're done with `consequent`.
                in_altr = 1; // Indicate we're working on `alternative`.
            } else { // No closing '}'; keep going.
                cons_argc++;
            }
        }
        if (in_cond) { // If we're after the `if` but before the `then`.
            if (strncmp(argv[i], "then", 5) == 0) { //Found closing `then`
                argv[i] = NULL; // Mark end of `char **condition`
                consequent = argv + i + 2; // Mark start of `consequent`.
                in_cond = 0; // Indicate we're done with `condition`.
                in_cons = 1; // Indicate we're working on `consequent`.
            } else { // No closing 'then'; keep going.
                cond_argc++;
            }
        }
    }
    // If any of the 3 parts are null, bail.
    if (condition == NULL || consequent == NULL || alternative == NULL) {
        print_if_usage();
        return 1; // In theory, this means it got invalid arguments.
    }
    // Execute the `condition` command, and react accordingly.
    if (execute(cond_argc, condition) == 0) { // If it run successfully...
        return execute(cons_argc, consequent); // Run the consequent.
    } else {
        return execute(altr_argc, alternative); // Run the alternative.
    }
}

static int handle_builtins(size_t argc, char **argv) {
    if (strncmp(argv[0], "exit", 5) == 0) { // exit builtin
        if (argc == 1) { // `exit` with no args is equivalent to `exit 0`.
            exit(0);
        } else { // `exit <value>` =>
            fputs("\nTODO: Set exit status to ", stdout);
            fputs(argv[1], stdout);
            fputs("\n", stdout);
            exit(123);
        }
    } else if (strncmp(argv[0], "if", 3) == 0) { // if builtin
        if (argc >= 10) { // if needs 10+ args.
            handle_if(argc, argv); // actually apply the if/else statement
        } else { // if there's not enough args, just print `if` usage info
            print_if_usage();
        }
        return 1; // handled by a builtin
    } else if (strncmp(argv[0], "setenv", 7) == 0) { // setenv builtin
        if (argc != 3) {
            fail("Usage: setenv NAME VALUE\n");
            return 1; // handled by a builtin.
        }
        setenv(argv[1], argv[2], 1 /* allow overwriting the value */);
        return 1; // handled by a builtin.
    }
    return 0; // not handled a builtin.
}

// Handle a line read via prompt().
static void handle(char buf[CHARS_PER_LINE]) {
    static char intbuf[INT_BUF_SIZE] = {0};
    static char tmp[CHARS_PER_LINE] = {0};

    size_t len = strlen(buf);
    if (len == 0) { // If it's empty, bail.
        return;
    }
    if (buf[len - 1] == '\n') { // Remove trailing newline, if it exists.
        buf[len - 1] = '\0';
    }
    if (strlen(buf) == 0) { // If it was _just_ a newline, bail.
        return;
    }
    // argv is a slot bigger than needed, and set to null bytes, so execvp
    // handles it better later on.
    char *argv[PARTS_PER_LINE + 1] = {0};
    size_t argc = shellsplit(argv, buf); // tokenize the line.
    handle_env_vars(argc, argv, tmp); // handle references to env vars.

    int status = execute(argc, argv); // execute the line.
    setenv("?", int_to_str(intbuf, status), 1); 
}

int main(int argc, char **argv) {
    char buf[CHARS_PER_LINE] = {0};
    int help = 0; // Non-zero means we should print help text below.
    int version = 0; // Non-zero means we should print version info below.
    for (int i = 1; i < argc; i++) { // Time for argument parsing!
        if (argv[i][0] != '-') { continue; } // Bail if arg is not flags.
        for (size_t j = 1; j < strlen(argv[i]); j++) {
            if (argv[i][j] == 'q') { settings.no_prompt = 1; }
            if (argv[i][j] == 'x') { settings.quick_exit = 1; }
            if (argv[i][j] == 'v') { version = 1; break; }
            if (argv[i][j] == 'h') { help = 1; break; }
        }
    }
    if (version) { // If they passed -v, print help text and bail.
        fputs("ish v" VERSION "\n", stdout);
        return 1;
    }
    if (help) { // If they passed -h or similar, print help text and bail.
        fputs("Usage: ish [-q] [-x] [-h]\n", stdout);
        fputs("-q    Quiet\n", stdout);
        fputs("-x    Exit immediately on error\n", stdout);
        fputs("-v    Print version information and exit\n", stdout);
        fputs("-h    Print help text and exit\n", stdout);
        return 1;
    }
    // Read prompt response, bail if we got NULL, handle line. Forever.
    while (prompt(buf) != NULL) { // NULL means EOF or error.
        handle(buf);
    }
    return 0;
}
