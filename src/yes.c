/**
 * NAME
 * ====
 *     yes - print a string repeatedly until killed
 *
 * SYNOPSIS
 * ========
 *     yes [STRING]
 *     yes [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Print a string repeatedly until killed.
 *
 *     STRING       The string to print. Default: "y".
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 *
 */


#include <stdio.h>
#include <string.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: yes [STRING]");
        puts("Print STRING (default: \"y\") repeatedly until killed\n");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    char *msg = "y";
    if (argc > 1)
        msg = argv[1];

    // Endlessly print all of the arguments.
    while(puts(msg) != EOF)
        ;
}
