/**
 * NAME
 * ====
 *     rmdir - remove empty directories
 *
 * SYNOPSIS
 * ========
 *     rmdir [-p] DIR...
 *     link [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Calls the rmdir function to remove the specified directories.
 *
 *     DIR          The pathname of an existent directory.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <err.h>
#include <libgen.h>
#include <stdio.h>
#include <unistd.h>
#include "boreutils.h"

static int rmdir_p(char *path) {
    char *dir = path;
    while (dir != NULL && (strncmp(dir, ".", 3) != 0)) {
        if (rmdir(dir) == -1) {
            warn("%s", path);
            return -1;
        }
        dir = dirname(dir);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: rmdir FILE");
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

    int dash_p = strncmp(argv[1], "-p", 3) == 0;
    int status = 0;

    if (!dash_p) {
        for (int i = 1; i < argc; i++) {
            if (rmdir(argv[i]) == -1) {
                perror(argv[0]);
                status = 1;
            }
        }
    } else {
        for (int i = 2; i < argc; i++) {
            if (rmdir_p(argv[i]) == -1) {
                status = 1;
            }
        }
    }

    return status;
}
