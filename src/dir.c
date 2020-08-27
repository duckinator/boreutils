/**
 * NAME
 * ====
 *     dir - list directory contents
 *
 * SYNOPSIS
 * ========
 *     dir [-a|-A] [-p] [DIRECTORY]
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
 *     -a           Show all files, including '.' or '..'.
 *     -A           Show all files, except '.' or '..'.
 *     -p           Append trailing slash to directories.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include "boreutils.h"

static int show_all = 0;
static int show_most = 0;

static int trailing_slash = 0;

static int filter(const struct dirent *de) {
    int hidden_file = (de->d_name[0] == '.');
    int is_dot_or_dotdot = hidden_file && (de->d_name[1] == '.' || de->d_name[1] == '\0');

    // If it's not hidden or we got -a, allow it.
    if (!hidden_file || show_all) {
        return 1;
    }

    // If we got -A and it's not . or .., allow it.
    if (show_most && !is_dot_or_dotdot) {
        return 1;
    }

    return 0;
}

static int is_dir(char *path) {
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

static int list_dir_contents(const char *dir) {
    struct dirent **entries;
    int num_entries = scandir(dir, &entries, filter, alphasort);
    if (num_entries < 0) {
        perror("dir");
        return 1;
    }

    for (int i = 0; i < num_entries; i++) {
        fputs(entries[i]->d_name, stdout);
        if (trailing_slash && is_dir(entries[i]->d_name)) {
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
        puts("Usage: dir [-a|-A] [-p] DIRECTORY");
        puts("List the contents of a directory.");
        puts("-a    Show all files, including '.' or '..'");
        puts("-A    Show all files, except '.' or '..'");
        puts("-p    Append trailing slash to directories");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    int start = 1;
    for (int i = 1; i < argc; i++) {
        // If the first character isn't a -, we're done looking at options.
        if (argv[i][0] != '-') {
            break;
        }
        start++;

        for (size_t j = 1; j < strlen(argv[i]); j++) {
            if (argv[i][j] == 'a') {
                show_all = 1;
                show_most = 0;
            }

            if (argv[i][j] == 'A') {
                show_most = 1;
            }

            if (argv[i][j] == 'p') {
                trailing_slash = 1;
            }
        }
    }

    if ((argc - start) == 0) {
        return list_dir_contents(".");
    }

    int show_header = ((argc - start) <= 2) ? 0 : 1;
    for (int i = start; i < argc; i++) {
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
