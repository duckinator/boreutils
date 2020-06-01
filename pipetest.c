#//\
    rm -f pipetest; clang -std=c11 pipetest.c -o pipetest && ./pipetest; STATUS=$?; rm pipetest; exit $STATUS

#include <stdio.h>      // perror
#include <stdlib.h>     // exit
#include <stdnoreturn.h>
#include <sys/types.h>  // pid_t
#include <unistd.h>     // close, dup2, execvp, fork


static void closefd(int fd) {
    if (close(fd) == -1) {
        perror("close");
    }
}

// Move oldfd to newfd.
static void redirect(int oldfd, int newfd) {
    if (oldfd == newfd) {
        return;
    }
    if (dup2(oldfd, newfd) == -1) {
        perror("dup2");
        exit(1);
    }
    closefd(oldfd); // successfully redirected
}

noreturn static void run(char **argv, int in, int out) {
    redirect(in, STDIN_FILENO);   /* <&in  : child reads from in */
    redirect(out, STDOUT_FILENO); /* >&out : child writes to out */

    if (execvp(argv[0], argv) == -1) {
        perror("run");
    }
    exit(1);
}

static void run_pipeline(char ***commands) {
    int in = STDIN_FILENO; // the first command reads
    for (size_t i = 0; commands[i] != NULL; i++) {
        if (commands[i + 1] == NULL) {
            run(commands[i], in, STDOUT_FILENO); /* $ command < in */
        }
        int fd[2]; // in/out pipe ends
        if (pipe(fd) == -1) {
            perror("pipe");
            exit(1);
        }

        pid_t pid = fork(); // child's pid
        if (pid == -1) {
            perror("fork");
            exit(1);
        }

        if (pid == 0) { // run command[i] in the child process
            closefd(fd[0]); // close unused read end of the pipe
            run(commands[i], in, fd[1]); // $ command < in > fd[1]
        } else { // parent
            closefd(fd[1]); // close unused write end of the pipe
            closefd(in);    // close unused read end of the previous pipe
            in = fd[0]; // the next command reads from here
        }
    }
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    char *a[] = {"echo", "hello", "world!", NULL};
    char *b[] = {"sed", "s/!//", NULL};
    char *c[] = {"tr", "l", "w", NULL};
    char **commands[] = {a, b, c, NULL};

    run_pipeline(commands);
}
