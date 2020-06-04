/**
 * NAME
 * ====
 *     head - copy the first part of files
 *
 * SYNOPSIS
 * ========
 *     head [-n NUMBER] [FILE...]
 *     head [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     For each FILE, read the first NUMBER lines and print them to standard output.
 *     If -n is not specified, it defaults to 10.
 *
 *     -n NUMBER    The NUMBER of lines to print.
 *     FILE         The file(s) to print.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/head.html

static int head_stream(FILE *stream, int lines) {
    int last;
    while (lines > 0) {
        last = fgetc(stream);
        if (last == EOF) {
            break;
        }
        fputc(last, stdout);
        if (last == '\n') {
            lines--;
        }
    }

    return 0;
}

static int head_file(char *path, int lines) {
    FILE *stream = fopen(path, "r");

    if (stream == NULL) {
        perror(path);
        return 1;
    }

    return head_stream(stream, lines);
}

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: head [-n NUMBER] [FILE...]");
        puts("Print the first NUMBER (10, if unspecified) lines of each FILE.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    int lines = 10;
    int offset = 0;

    // We use `i < argc - 1` because we access `argv[i + 1]` in the loop.
    for (int i = 1; i < argc - 1; i++) {
        if (strncmp(argv[i], "-n", 3) == 0) {
            offset = i + 1;
            if (argc <= i) {
                bu_invalid_argument(argv[i], "-n needs a number");
                return 1;
            }
            lines = atoi(argv[i + 1]);
            break;
        }
    }

    if ((argc - offset) < 2) {
        return head_stream(stdin, lines);
    }

    for (int i = 1 + offset; i < argc; i++) {
        int ret = head_file(argv[i], lines);
        if (ret != 0) {
            return ret;
        }
    }

    return 0;
}
