/**
 * NAME
 * ====
 *     man - print manual pages
 *
 * SYNOPSIS
 * ========
 *     man NAME...
 *     man -k NAME...
 *     man [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     In the first invocation (no `-k`), prints the specified manual page(s).
 *
 *     In the second invocation (with `-k`), searches for the specified page(s).
 *
 *     -k           Search for the specified page(s), instead of printing
 *                  their contents.
 *
 *     NAME         The manual page(s) to print.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "boreutils.h"

#ifndef BU_MAN_PREFIX
#   define BU_MAN_PREFIX "./doc/"
#endif

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: man [-k] PAGE...");
        puts("");
        puts("Prints the specified manual page(s).");
        puts("");
        puts("PAGE  The page(s) to print.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc < 2) {
        bu_missing_argument(argv[0]);
        return 1;
    }

    char path[BU_PATH_BUFSIZE] = BU_MAN_PREFIX;
    strncpy(path + strlen(path), argv[1], sizeof(path) - strlen(path));
    strncpy(path + strlen(path), ".rst", sizeof(path) - strlen(path));

    int fd = open(path, O_RDONLY);

    char buf[8192] = {0};
    ssize_t bytes_read = 1;
    while (bytes_read > 0) {
        bytes_read = read(fd, buf, sizeof(buf));
        fputs(buf, stdout);
        memset(buf, 0, sizeof(buf));
    }

    return 0;
}
