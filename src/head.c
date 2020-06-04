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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/head.html

static int head_stream(FILE *stream, int lines) {
    char *line = NULL;
    size_t n = 0;
    ssize_t bytes_read = 1;
    while (bytes_read >= 0 && lines > 0) {
        bytes_read = getline(&line, &n, stream);
        if (bytes_read > 0 && line != NULL) {
            fputs(line, stdout);
            lines--;
        }
    }

    if (line != NULL) {
        free(line);
    }

    if (bytes_read == -1) {
        perror("head: error reading file");
        return 1;
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


    // From the POSIX.1-2017 standard:
    // https://pubs.opengroup.org/onlinepubs/9699919799/utilities/head.html#tag_20_57_10
    //
    // >  The standard output shall contain designated portions of the input files.
    // > If multiple file operands are specified, head shall precede the output for each with the header:
    // > "\n==> %s <==\n", <pathname>
    // > except that the first header written shall not include the initial <newline>.
    //
    // `print_file_headers` is true if there's >2 args after the flags.

    int print_file_headers = (argc - offset) > 2;
    int ret;
    for (int i = 1 + offset; i < argc; i++) {
        if (print_file_headers && i != (1 + offset)) {
            puts(""); // print leading newline for all but the first header
        }
        if (strncmp(argv[i], "-", 2) == 0) {
            if (print_file_headers) {
                puts("==> standard input <==");
            }
            ret = head_stream(stdin, lines);
        } else {
            if (print_file_headers) {
                fputs("==> ", stdout);
                fputs(argv[i], stdout);
                puts(" <==");
            }
            ret = head_file(argv[i], lines);
        }
        if (ret != 0) {
            return ret;
        }
    }

    return 0;
}
