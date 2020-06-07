/**
 * NAME
 * ====
 *     link - calls the link function
 *
 * SYNOPSIS
 * ========
 *     link FILE1 FILE2
 *     link [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     It literally just calls link(FILE1, FILE2).
 *
 *     FILE1        The pathname of an existing file.
 *     FILE2        The pathname of the link to create.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 *
 */


#include <stdio.h>
#include <unistd.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: link FILE1 FILE2");
        puts("Creates a link to existing path FILE1, named FILE2");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc < 3) {
        bu_missing_argument(argv[0]);
        return 1;
    }

    if (argc > 3) {
        bu_extra_argument(argv[0]);
        return 1;
    }

    if (link(argv[1], argv[2]) == -1) {
        perror(argv[0]);
        return 1;
    }

    return 0;
}
