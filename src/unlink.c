/**
 * NAME
 * ====
 *     unlink - calls the unlink function
 *
 * SYNOPSIS
 * ========
 *     unlink FILE
 *     link [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Calls the unlink function to remove the specified file.
 *
 *     FILE         The pathname of an existing file.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include <unistd.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: unlink FILE");
        puts("Removes an existing file path, FILE.");
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

    if (unlink(argv[1]) == -1) {
        perror(argv[0]);
        return 1;
    }

    return 0;
}
