/**
 * NAME
 * ====
 *     sleep - delay for a specified number of seconds
 *
 * SYNOPSIS
 * ========
 *     sleep TIME
 *     sleep [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Wait at least TIME seconds, then exit.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/sleep.html

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: sleep TIME");
        puts("Wait at least TIME seconds, then exit.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc < 2) {
        bu_missing_argument(argv[0]);
        return 1;
    }

    if (argc > 2) {
        bu_extra_argument(argv[0]);
        return 1;
    }

    int time_i = atoi(argv[1]);
    unsigned int time = (unsigned int)(time_i < 0 ? -time_i : time_i);
    sleep(time);

    return 0;
}
