/**
 * NAME
 * ====
 *     true - do nothing and succeed.
 *
 * SYNOPSIS
 * ========
 *     true [ignored arguments]
 *     true [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Exits successfully.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/true.html

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: true");
        puts("Do nothing and succeed.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    return 0;
}
