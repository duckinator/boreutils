/**
 * NAME
 * ====
 *     tty - prints the file name of the terminal connected to standard input
 *
 * SYNOPSIS
 * ========
 *     pwd [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Prints the file name of the terminal connected to standard input.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include <unistd.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/tty.html

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s [-s]\n\n", argv[0]);
        printf("Print the file name of the terminal connected to standard input.\n\n");
        printf("Options:\n");
        printf("  -s    Don't print anything, only return an exit status.\n");
        return 1;
    }

    int dash_s = has_arg(argc, argv, "-s");

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if ((argc > 2) || ((argc > 1) && !dash_s)) {
        bu_extra_argument(argv[0]);
        return 1;
    }


    if (isatty(STDIN_FILENO)) {
        if (!dash_s) {
            printf("%s\n", ttyname(STDIN_FILENO));
        }
        return 0;
    }

    return 1;
}
