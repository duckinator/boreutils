/**
 * NAME
 * ====
 *     cat - concatenate files and print them
 *
 * SYNOPSIS
 * ========
 *     cat [-u] FILE...
 *     cat [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Concatenate files and print them to standard output.
 *
 *     -u           Don't buffer the output.
 *
 *     FILE         The file(s) to print.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/cat.html

static int cat_fd(int fd) {
    // Size is arbitrary; feel free to adjust it if needed.
    char buf[4096] = {0};
    ssize_t bytes_read = 1;
    while (bytes_read > 0) {
        bytes_read = read(fd, buf, sizeof(buf) - 1);
        buf[bytes_read] = 0;
        fputs(buf, stdout);
    }

    if (bytes_read == -1) {
        perror("cat: error reading file");
        return 1;
    }

    return 0;
}

static int cat_file(char *path) {
    int fd = open(path, O_RDONLY);

    if (fd == -1) {
        perror("cat: error opening file");
        return 1;
    }

    return cat_fd(fd);
}

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: cat [-u] FILE...");
        puts("");
        puts("Concatenate and print the specified file(s).");
        puts("");
        puts("FILE    The file(s) to print.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc < 2) {
        return cat_fd(STDIN_FILENO);
    }

    for (int i = 1; i < argc; i++) {
        int ret = cat_file(argv[i]);
        if (ret != 0) {
            return ret;
        }
    }

    return 0;
}
