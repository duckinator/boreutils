/**
 * NAME
 * ====
 *     who - display who is on the system
 *
 * SYNOPSIS
 * ========
 *     who [-mTu]
 *
 * DESCRIPTION
 * ===========
 *     Print information about users that are on the system.
 *
 *     -m           Print information about only the current terminal.
 *     -T           Show the state of each terminal.
 *     -u           Include the "idle time" for each displayed user.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/who.html

int main(int argc, char **argv) {
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: who [-mTu]");
        puts("Print information about users that are on the system.");
        return 1;
    }

    int dash_m = 0;
    int dash_T = 0;
    int dash_u = 0;

    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        if (arg[0] != '-') {
            bu_invalid_argument(argv[0], argv[i]);
            return 1;
        }

        arg++;
        for (; *arg; arg++) {
            char flag = *arg;

            if (flag == 'm') { dash_m = 1; }
            else if (flag == 'T') { dash_T = 1; }
            else if (flag == 'u') { dash_u = 1; }
            else {
                char tmp[3] = "-X";
                tmp[1] = *arg;
                bu_invalid_argument(argv[0], tmp);
                return 1;
            }
        }
    }


    char *user_name = "USERNAME";
    char terminal_state = '?';
    char *terminal_name = "TERMNAME";
    char *time_of_login = "TIME";
    char *idle_time;

    if (dash_T) {
        // Actual terminal state.
        // +        allows write access to other users
        // -        denies write access to other users
        // ?        cannot be determined
        // <space>  not associated with a terminal
        // TODO: Actually set terminal_state.
    }

    if (dash_u) {
        idle_time = " IDLE";
    } else {
        idle_time = "";
    }

    if (dash_T) {
        printf("%s %c %s %s%s\n", user_name, terminal_state, terminal_name, time_of_login, idle_time);
    } else {
        printf("%s %s %s%s\n", user_name, terminal_name, time_of_login, idle_time);
    }

    return 0;
}
