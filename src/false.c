/**
 * NAME
 * ====
 *     false - do nothing and fail.
 *
 * SYNOPSIS
 * ========
 *     false [ignored arguments]
 *     false [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Exits unsuccessfully.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/false.html

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: false");
        puts("Do nothing and fail.");
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    return 1;
}
