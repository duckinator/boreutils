/**
 * NAME
 * ====
 *     mkdir - make directories
 *
 *
 * SYNOPSIS
 * ========
 *     mkdir [-p] [-m MODE] DIR
 *     mkdir [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     
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
#include <stdlib.h>
#include <sys/stat.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: mkdir [-p] [-m MODE] DIR");
        puts("Creates a directory at DIR.");
        puts("-p        Create parent directories.");
        puts("-m MODE   Set the file permission bits.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc < 2) {
        bu_missing_argument(argv[0]);
        return 1;
    }

    char *path = NULL;
    mode_t mode = S_IRWXU | S_IRWXG | S_IRWXO;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // Parse flags.
            fputs("mkdir: error: doesn't support any flags yet.", stderr);
            return 1;
        } else if (path == NULL) {
            // First non-flag is the directory to create.
            path = argv[i];
        } else {
            // We got more than one path!
            // But POSIX only requires one, and I'm lazy, so we bail.
            bu_extra_argument(argv[0]);
            return 1;
        }
    }

    if (mkdir(path, mode) == -1) {
        perror(argv[0]);
        return 1;
    }

    return 0;
}
