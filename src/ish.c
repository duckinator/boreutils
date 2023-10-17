/**
 * NAME
 * ====
 *     ish - a vaguely shell-ish program
 *
 * SYNOPSIS
 * ========
 *     ish [-q] [-x] [SCRIPT_PATH] [ARGS...]
 *     ish [-h|-v]
 *
 *     -q    Don't display the prompt.
 *     -x    Exit immediately on error
 *     -v    Print version information and exit
 *     -h    Print help text and exit
 *
 * USAGE
 * =====
 *     cd DIRECTORY         Changes the current directory to DIRECTORY.
 *                          If DIRECTORY is -, changes to ${OLDPWD} instead.
 *     exit [STATUS]        Exits with the specified STATUS number. (Default=0)
 *     setenv NAME VALUE    Sets ${NAME} to "VALUE".
 *     ${NAME}              Is replaced with the value of env var ${NAME}
 *     "foo"                Double-quoted string.
 *     'foo'                Single-quoted string.
 *     "foo""bar"'baz'      Combined into one string; equivalent to "foobarbaz"
 *     foo | bar | baz      Basic pipe support; redirects stdout to stdin.
 *
 * If statements:
 *     if THIS-RETURNS-ZERO then { RUN-THIS } else { RUN-THIS-INSTEAD }
 *
 * No support for:
 *     - combining if/else and pipes
 *     - boolean operators (&& || & | etc)
 *     - math
 *     - subshells
 *     - complex redirection
 */
#define VERSION "0.0.1"

#include <stdio.h>      // fputs, fgets, perror, stdout, stderr
#include <stdlib.h>     // atoi, exit, getenv, setenv
#include <string.h>     // strlen, strncmp
#include <sys/types.h>  // pid_t
#include <sys/wait.h>   // waitpid, WEXITSTATUS, WIFEXITED, WIFSIGNALED, WTERMSIG, WUNTRACED
#include <unistd.h>     // close, dup2, execvp, fork

#define INT_BUF_SIZE 22 // 20 (max digits in int64) + 1 (sign) + 1 (null)
#define CHARS_PER_LINE (32 * 1024) // Max chars per line of input
#define PARTS_PER_LINE 512         // Max words per line.
#define PIPELINE_PARTS 32          // Max pipe segments per line.

typedef char *PipelineToken;
typedef struct PipelinePart_s {
    PipelineToken tokens[PARTS_PER_LINE + 1];
    size_t argc;
} PipelinePart;
typedef struct Pipeline_s {
    PipelinePart commands[PIPELINE_PARTS + 1];
} Pipeline;
static int execute(Pipeline *pipeline);
static struct Settings_s { // `settings` variable holds all the settings.
    int no_prompt;
    int quick_exit;
} settings = {0};
static int update_pwd(void) {
    char path_buf[8192] = {0};
    if (getcwd(path_buf, sizeof(path_buf)) == NULL) {
        perror("ish: update_cwd()");
        return -1;
    }
    setenv("PWD", path_buf, 1);
    return 0;
}
// Print the prompt (unless settings.no_prompt), return next line of input
static char *prompt(char buf[CHARS_PER_LINE]) {
    if (!settings.no_prompt) {
        char path_buf[8192] = {0};
        if (getcwd(path_buf, sizeof(path_buf)) == NULL) {
            perror("ish: prompt(): getcwd");
        } else {
            fputs(path_buf, stdout);
        }
        fputs("$ ", stdout);
    }
    return fgets(buf, CHARS_PER_LINE, stdin);
}
static void fail(char *msg) { // Print `msg` to stderr and set $? to 1.
    fputs(msg, stderr);
    setenv("?", "1", 1);
}
static void print_if_usage(void) { // Explain how if statements work.
    fail("Usage: if CONDITION then { CONSEQUENT } else { ALTERNATIVE }\n");
}
static void closefd(int fd) { // Close fd and print an error if it failed
    if (close(fd) == -1) { perror("close"); }
}
static void redirect(int oldfd, int newfd) { // Move oldfd to newfd.
    if (oldfd == newfd) { // If they're the same, nothing to do.
        return;
    }
    if (dup2(oldfd, newfd) == -1) {
        perror("dup2");
        exit(1);
    }
    closefd(oldfd); // successfully redirected
}
// If whole token is "${X}", replace with the value of the env variable X.
static void expand_env_vars(PipelinePart *command, char scratch[CHARS_PER_LINE]) {
    for (size_t i = 0; i < command->argc && command->tokens[i]; i++) {
        char *tmp = command->tokens[i];
        if (tmp[0] == '$' && tmp[1] == '{' && tmp[strlen(tmp) - 1] == '}') {
            strncpy(scratch, tmp + 2, CHARS_PER_LINE);
            scratch[strlen(scratch) - 1] = '\0';
            command->tokens[i] = getenv(scratch);
        }
    }
}
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
static size_t shellsplit(Pipeline *pipeline, char input[CHARS_PER_LINE]) {
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
    size_t pipeline_idx = 0;
    pipeline->commands[pipeline_idx].tokens[0] = input + buf_idx;
    for (; input_idx < strlen(input); input_idx++) {
        PipelinePart *command = &pipeline->commands[pipeline_idx];
        int is_pipe = (input[input_idx] == '|');
        int is_dquote = (input[input_idx] == '"');
        int is_squote = (input[input_idx] == '\'');
        int is_space = (input[input_idx] == ' ');
        int break_pipe = !in_dquote && !in_squote && is_pipe;
        int new_token = !in_dquote && !in_squote && is_space;
        int consume = !(is_dquote || is_squote || is_space || is_pipe) ||
            (in_dquote && !is_dquote) || (in_squote && !is_squote);

        if (break_pipe) {
            command->tokens[num_pieces - 1] = NULL;
            command->argc = num_pieces;
            num_pieces = 0;
            pipeline_idx++;
            command = &pipeline->commands[pipeline_idx];
        }
        if (is_dquote && !in_squote) { in_dquote = !in_dquote; }
        if (is_squote && !in_dquote) { in_squote = !in_squote; }
        if (consume) { // Consume the token.
            buf[buf_idx] = input[input_idx];
            buf_idx++;
        }
        if (new_token) { // Start a new token.
            if (input[input_idx + 1] == ' ') { // Eat extra spaces.
                input_idx++;
                buf_idx++;
                continue;
            }
            num_pieces++;
            command->tokens[num_pieces - 1] = input + buf_idx + 1;
            buf[buf_idx] = '\0';
            buf_idx++;
        }
    }
    pipeline->commands[pipeline_idx].argc = num_pieces;
    buf[buf_idx] = '\0';
    memcpy(input, buf, buf_idx + 1);
    return num_pieces;
}
// Helper function: convert argc+argv to a Pipeline, then execute it.
static int execute_a(size_t argc, char **argv) {
    Pipeline pipeline = {{{{0}, 0}}};
    for (size_t i = 0; i < argc; i++) {
        pipeline.commands[0].tokens[i] = argv[i];
    }
    pipeline.commands[0].tokens[argc] = NULL;
    pipeline.commands[1].tokens[0] = NULL;
    return execute(&pipeline);
}
static int execute_if(size_t argc, char **argv) { // Run if/else statements
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
    if (execute_a(cond_argc, condition) == 0) { // If it runs successfully
        return execute_a(cons_argc, consequent); // Run the consequent.
    } else {
        return execute_a(altr_argc, alternative); // Run the alternative.
    }
}
static int handle_builtins(Pipeline *pipeline) { // Run builtin commands
    PipelinePart *command = &pipeline->commands[0];
    if (strncmp(command->tokens[0], "cd", 3) == 0) { // cd builtin
        if (command->argc != 2) {
            fail("Usage: cd PATH\n");
            return -1; // builtin encountered error
        } else { // ish only supports `cd <path>`
            char path_buf[8192] = {0};
            if (getcwd(path_buf, sizeof(path_buf)) == NULL) {
                // If we get here, the CWD/PWD probably doesn't exist anymore.
                strncpy(path_buf, getenv("PWD"), sizeof(path_buf));
            }
            if (strncmp(command->tokens[1], "-", 2) == 0) {
                strncpy(command->tokens[1], getenv("OLDPWD"), sizeof(path_buf));
            }
            if (chdir(command->tokens[1]) < 0) {
                perror("cd");
                return -1; // builtin encountered error
            }
            setenv("OLDPWD", path_buf, 1); // successfully changed cwd.
            if (update_pwd() == -1) {
                return -1;
            }
        }
        return 1; // handled by builtin
    } else if (strncmp(command->tokens[0], "exit", 5) == 0) { // exit builtin
        if (command->argc == 1) { // `exit` with no args == `exit 0`.
            exit(0);
        } else { // `exit <value>` => exit(<value as int>)
            exit(atoi(command->tokens[1]));
        }
    } else if (strncmp(command->tokens[0], "if", 3) == 0) { // if builtin
        if (command->argc >= 10) { // if 10+ args are provided, run it.
            execute_if(command->argc, command->tokens);
        } else { // if there's not enough args, just print `if` usage info
            print_if_usage();
            return -1; // builtin encountered error
        }
        return 1; // handled by a builtin
    } else if (strncmp(command->tokens[0], "setenv", 7) == 0) { // setenv
        if (command->argc != 3) {
            fail("Usage: setenv NAME VALUE\n");
            return -1; // builtin encountered error
        }
        setenv(command->tokens[1], command->tokens[2], 1 /* overwrite */);
        return 1; // handled by a builtin.
    }
    return 0; // not handled a builtin.
}
// Given argv, in, and out, run it. If it fails, print an error.
static void run(char **argv, int in, int out) {
    redirect(in, STDIN_FILENO);   // child reads from `in`
    redirect(out, STDOUT_FILENO); // child writes to `out`
    if (execvp(argv[0], argv) == -1) {
        perror(argv[0]);
    }
    exit(1);
}
static void run_pipeline(Pipeline *pipeline) { // Run an entire pipeline.
    char env_scratch[CHARS_PER_LINE] = {0}; // Buffer for expand_env_vars.
    int in = STDIN_FILENO; // The first command reads stdin directly.

    // Loop through the commands in the pipeline and run them.
    for (size_t i = 0; pipeline->commands[i].tokens[0] != NULL; i++) {
        if (pipeline->commands[i + 1].tokens[0] == NULL) {
            // This is the last command. Run it without forking.
            PipelinePart *command = &pipeline->commands[i];
            expand_env_vars(command, env_scratch);
            run(command->tokens, in, STDOUT_FILENO); // command < in
        }
        // If we get here, this isn't the last command.
        int fd[2]; // in/out pipe ends.
        if (pipe(fd) == -1) { // Set up I/O pipes.
            perror("pipe");
            exit(1);
        }
        pid_t child_pid = fork(); // child's pid
        if (child_pid == -1) { // bail immediately if an error occurred
            perror("fork");
            exit(1);
        } else if (child_pid == 0) { // child process runs command[i]
            closefd(fd[0]); // close unused read end of the pipe
            PipelinePart *command = &pipeline->commands[i];
            expand_env_vars(command, env_scratch);
            run(command->tokens, in, fd[1]); // command < in > fd[1]
        } else { // Parent process
            closefd(fd[1]); // close unused write end of the pipe
            closefd(in);    // close unused read end of the previous pipe
            in = fd[0]; // the next command reads from here
        }
    }
}
static int execute(Pipeline *pipeline) { // Run command + return exit code
    // NOTE: If you're using builtins you can NOT use pipes, currently.
    int bi_status = handle_builtins(pipeline);
    if (bi_status < 0) {
        return 1; // encountered error
    } else if (bi_status > 0) {
        return 0; // successfully handled by builtin
    }
    pid_t child_pid = fork();
    int status;
    if (child_pid == -1) { // bail immediately if an error occurred
        perror("fork");
        exit(1);
    } else if (child_pid == 0) {
        run_pipeline(pipeline);
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
static void handle(char buf[CHARS_PER_LINE]) { // Handle a line of input.
    static char intbuf[INT_BUF_SIZE] = {0};
    size_t len = strlen(buf);
    if (len == 0) { // If it's empty, bail.
        return;
    }
    if (buf[len - 1] == '\n') { // Remove trailing newline, if it exists.
        buf[len - 1] = '\0';
    }
    if (strlen(buf) == 0) { // If it was _only_ a newline, bail.
        return;
    }
    Pipeline pipeline = {{{{0}, 0}}};
    shellsplit(&pipeline, buf); // tokenize the line.
    int status = execute(&pipeline);
    setenv("?", int_to_str(intbuf, status), 1); 
}
static void set_default_variables(int argc, char **argv) {
    char intbuf[INT_BUF_SIZE] = {0};
    int offset = 0;
    setenv("SHELL", argv[0], 1); // set ${SHELL}
    for (int i = 0; i < argc; i++) { // set ${0}, ${1}, ..., ${<argc - 1>}
        if (argv[i][0] == '-') { offset++; continue; } // skip flags.
        setenv(int_to_str(intbuf, i - offset), argv[i], 1);
    }
}
int main(int argc, char **argv) {
    char input[CHARS_PER_LINE] = {0};
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
        fputs("Usage: ish [-q] [-x] [-h] [-v]\n", stdout);
        fputs("-q    Quiet\n", stdout);
        fputs("-x    Exit immediately on error\n", stdout);
        fputs("-v    Print version information and exit\n", stdout);
        fputs("-h    Print help text and exit\n", stdout);
        return 1;
    }

    set_default_variables(argc, argv);

    if (update_pwd() == -1) {
        // if updating $PWD resulted in an error, try to `cd ${HOME}`
        char *home_dir = getenv("HOME");
        if ((home_dir == NULL) || (chdir(home_dir) == -1)) {
            chdir("/"); // if $HOME isn't set or there was an error, `cd /`
        }
        update_pwd(); // assume it worked this time.
    }

    while (prompt(input) != NULL) { // Prompt + read input, bail if NULL.
        handle(input);
    }
    return 0;
}
