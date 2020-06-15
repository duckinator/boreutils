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


static int path_exists(char *pathname) {
  struct stat buffer;
  return stat(pathname, &buffer);
}


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
    int dash_p = 0;
    int dash_m = 0;

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            // Parse flags.
            if (argv[i][1] == 'p') {
                dash_p = 1;
            } else if (argv[i][1] == 'm') {
                dash_m = 1;
            } else {
                char str[3] = "-X";
                str[1] = argv[i][1];
                bu_invalid_argument(argv[0], str);
                return 1;
            }
        } else if (path == NULL) {
            // First non-flag is the directory to create.
            path = argv[i];
        } else {
            // We got more than one path!
            // But POSIX only requires handling one, and I'm lazy, so we bail.
            bu_extra_argument(argv[0]);
            return 1;
        }
    }

    // mkdir requires a path.
    if (path == NULL) {
        bu_missing_argument(argv[0]);
        return 0;
    }

    // TODO: Figure out how to parse the mode.
    if (dash_m) {
        fputs("mkdir: error: doesn't support -m MODE yet.", stderr);
        return 1;
    }

    if (dash_p) {
        size_t len = strlen(path);
        for (size_t i = 0; i < len; i++) {
            if (path[i] != '/') {
                continue;
            }

            // skip the first and last character
            if (i == 0 || i == (len - 1)) {
                continue;
            }
            path[i] = '\0';
            if (path_exists(path) == -1) {
                if (mkdir(path, mode) == -1) {
                    perror(argv[0]);
                    return 1;
                }
            }
            path[i] = '/';
        }
    }

    if (mkdir(path, mode) == -1) {
        perror(argv[0]);
        return 1;
    }

    return 0;
}
