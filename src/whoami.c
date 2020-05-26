/**
 * NAME
 * ====
 *     whoami - print effective user ID.
 *
 * SYNOPSIS
 * ========
 *     whoami
 *     whoami [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Print the effective user ID. Equivalent to `id -un`.
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
        puts("Usage: whoami");
        puts("Print effected userid.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc > 1) {
        bu_extra_argument(argv[0]);
        return 1;
    }

    char *ret = getlogin();

    if (ret == NULL) {
        perror(argv[0]);
        return 1;
    }

    puts(ret);

    return 0;
}
