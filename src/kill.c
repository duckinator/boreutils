/**
 * NAME
 * ====
 *     kill - terminate or send a signal to processes
 *
 * SYNOPSIS
 * ========
 *     kill [-s SIGNAL_NAME|-SIGNAL_NAME|-SIGNAL_NUMBER] PID...
 *     kill -l [EXIT_STATUS]
 *
 * DESCRIPTION
 * ===========
 *     If -s, -SIGNAL_NAME, or -SIGNAL_NUMBER is provided:
 *         For each PID provided, send the specified signal to the process.
 *         The -SIGNAL_NAME argument is equivalent to -s SIGNAL_NAME.
 *
 *     If -l is provided, but no EXIT_STATUS is provided:
 *         Print all supported values of SIGNAL_NAME.
 *
 *     If -l EXIT_STATUS is provided:
 *         If EXIT_STATUS is the value of a signal number, the corresponding
 *         SIGNAL_NAME for that is printed.
 *
 *     SIGNAL_NAME      The name of the signal to send.
 *     SIGNAL_NUMBER    The signal number to send.
 *
 *     --help           Print help text and exit.
 *     --version        Print version information and exit.
 */


#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/kill.html

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: kill -s SIGNAL_NAME PID...");
        puts("       kill -l [EXIT_STATUS]");
        puts("       kill [-SIGNAL_NAME] PID...");
        puts("       kill [-SIGNAL_NUMBER] PID...");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc < 2) {
        bu_missing_argument(argv[0]);
        return 1;
    }

    #define NUM_SIGNALS 33 // No idea how high the values go.
    char *signal_names[NUM_SIGNALS] = {0};

    // It really feels like there should be a function for this?
    signal_names[SIGABRT] = "SIGABRT";
    signal_names[SIGALRM] = "SIGALRM";
    signal_names[SIGBUS] = "SIGBUS";
    signal_names[SIGCHLD] = "SIGCHLD";
    signal_names[SIGCONT] = "SIGCONT";
    signal_names[SIGFPE] = "SIGFPE";
    signal_names[SIGHUP] = "SIGHUP";
    signal_names[SIGILL] = "SIGILL";
    signal_names[SIGINT] = "SIGINT";
    signal_names[SIGKILL] = "SIGKILL";
    signal_names[SIGPIPE] = "SIGPIPE";
    signal_names[SIGQUIT] = "SIGQUIT";
    signal_names[SIGSEGV] = "SIGSEGV";
    signal_names[SIGSTOP] = "SIGSTOP";
    signal_names[SIGTERM] = "SIGTERM";
    signal_names[SIGTSTP] = "SIGTSTP";
    signal_names[SIGTTIN] = "SIGTTIN";
    signal_names[SIGTTOU] = "SIGTTOU";
    signal_names[SIGUSR1] = "SIGUSR1";
    signal_names[SIGUSR2] = "SIGUSR2";
#ifdef SIGPOLL
    signal_names[SIGPOLL] = "SIGPOLL";
#endif
#ifdef SIGPROF
    signal_names[SIGPROF] = "SIGPROF";
#endif
#ifdef SIGSYS
    signal_names[SIGSYS] = "SIGSYS";
#endif
    signal_names[SIGTRAP] = "SIGTRAP";
    signal_names[SIGURG] = "SIGURG";
#ifdef SIGVTALRM
    signal_names[SIGVTALRM] = "SIGVTALRM";
#endif
    signal_names[SIGXCPU] = "SIGXCPU";
    signal_names[SIGXFSZ] = "SIGXFSZ";

    int signal_num = -1;
    char *signal_name = NULL;
    char *pid_str = NULL;
    int dash_s = 0;
    int dash_l = 0;
    char *exit_status = NULL;

    if (strncmp(argv[1], "-s", 3) == 0) {
        // `kill -s SIGNAL_NAME PID`
        if (argc < 4) {
            bu_missing_argument(argv[0]);
            return 1;
        }
        dash_s = 1;
        signal_name = argv[2];
        pid_str = argv[3];
    } else if (strncmp(argv[1], "-l", 3) == 0) {
        // `kill -l [EXIT_STATUS]`
        dash_l = 1;
        if (argc > 2) {
            exit_status = argv[2];
        }
    } else if (argv[1][0] == '-') {
        // `kill -SIGNAL_NAME PID`
        // The +1 skips the leading dash.
        signal_name = argv[1] + 1;
        pid_str = argv[2];
    } else {
        // Anything else is invalid.
        bu_invalid_argument(argv[0], argv[1]);
    }

    if (dash_l) {
        if (exit_status) {
            char *endp;
            errno = 0;
            signal_num = strtol(exit_status, &endp, 10);
            if (errno || *endp != 0) {
                printf("unknown signal: %s\n", exit_status);
                return 1;
            }

            // Detect if the exit status given as argument
            // has the "special" bit set that shells (?) like to set
            // whenever an process died from a signal.
            if (signal_num >= NUM_SIGNALS && (signal_num & 128) == 128)
                signal_num -= 128;

            if (signal_num == 0) {          
                // procps-ng `kill` says EXIT, but POSIX *explicitly* states
                // that `kill -l 0` should print "0".
                puts("0");
            } else if (signal_num < NUM_SIGNALS  && signal_names[signal_num] != NULL) {
                // `kill -l EXIT_STATUS` prints names without the SIG prefix.
                printf("%s\n", signal_names[signal_num] + 3);
            } else {
                printf("unknown signal: %d\n", signal_num);
                return 1;
            }
        } else {
            int i_per_line = 0;
            for (int i = 0; i < NUM_SIGNALS; i++) {
                if (signal_names[i] != NULL) {
                    i_per_line++;
                    printf("%i) %s\t", i, signal_names[i]);
                }
                if (i_per_line > 3) {
                    puts("");
                    i_per_line = 0;
                }
            }
        }
        return 0;
    }

    // If signal_name is a string containing a number, convert it to an int.
    if (signal_name != NULL && signal_name[0] >= '0' && signal_name[0] <= '9') {
        signal_num = atoi(signal_name);
        signal_name = NULL;
    }

    // If signal_name contains a signal name, find the number for it.
    if (signal_name) {
        for (int i = 0; i < NUM_SIGNALS; i++) {
            if (signal_names[i] != NULL && strcmp(signal_names[i] + 3, signal_name) == 0) {
                signal_num = i;
            }
        }
    }

    if (signal_name != NULL && signal_num == -1) {
        fputs(argv[0], stderr);
        fputs(": Unknown signal: ", stderr);
        fputs(signal_name, stderr);
        fputs("\n", stderr);
    }

    pid_t pid = -1;
    if (pid_str) {
        pid = atoi(pid_str);
    }

    if (kill(pid, signal_num) == -1) {
        perror(argv[0]);
    }

    return 0;
}
