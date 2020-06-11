/**
 * NAME
 * ====
 *     dir - list directory contents
 *
 * SYNOPSIS
 * ========
 *     dir [DIRECTORY]
 *     dir [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     `dir` lists the contents of the specified DIRECTORY, or the current
 *     working directory if that's not specified.
 *
 *     Given the complexity of `ls`, this was provided as a stopgap until
 *     ls is implemented.
 *
 *     DIRECTORY    The path to a directory.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "boreutils.h"

static int is_dir(char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

static int list_dir_contents(const char *dir) {
    struct dirent **entries;
    int num_entries = scandir(dir, &entries, 0, alphasort);
    if (num_entries < 0) {
        perror("dir");
        return 1;
    }

    for (int i = 0; i < num_entries; i++) {
        fputs(entries[i]->d_name, stdout);
        if (is_dir(entries[i]->d_name)) {
            fputc('/', stdout);
        }
        puts("");
        free(entries[i]);
    }
    free(entries);

    return 0;
}

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: dir DIRECTORY");
        puts("List the contents of a directory.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc == 1) {
        return list_dir_contents(".");
    }

    int show_header = (argc <= 2) ? 0 : 1;
    for (int i = 1; i < argc; i++) {
        if (show_header) {
            fputs(argv[i], stdout);
            puts(":");
        }

        int ret = list_dir_contents(argv[i]);
        if (ret != 0) {
            return ret;
        }

        // If there's more directories left, print an extra newline.
        if (i < (argc - 1)) {
            puts("");
        }
    }

    return 0;
}
