/**
 * NAME
 * ====
 *     which - show the full path of executables
 *
 * SYNOPSIS
 * ========
 *     which [-as] PROGRAM...
 *     which [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     For each PROGRAM argument, search the PATH for an executable with
 *     that file name, and print the full path to standard output.
 *
 *     -a           Print all matching executables (not just the first).
 *     -s           No output. Returns 0 if all PROGRAMs are found, 1 otherwise.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 *
 *     PROGRAM  The file name of a program to locate.
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "boreutils.h"

// `which` is not standardized. This implementation is inspired by FreeBSD's.

#define MAX_PATH_PARTS 1024 // see parse_path();

typedef struct Path_s {
    char *parts[MAX_PATH_PARTS];
    char *_path;
    size_t size;
} Path;

static int parse_path(Path *path_obj) {
    char *path;
    char *tmp = getenv("PATH");

    if (tmp == NULL) {
        return -1;
    }

    size_t length = strlen(tmp) + 1;
    path = malloc(sizeof(char) * length);
    memset(path, 0, length);
    strncpy(path, tmp, length);

    path_obj->size = 1;
    path_obj->_path = path;
    path_obj->parts[0] = path_obj->_path;
    for (size_t i = 0; i <= length; i++) {
        if (path[i] == ':') {
            path[i] = '\0';
            path_obj->parts[path_obj->size] = path + i + 1;
            path_obj->size++;
        }

        if (path_obj->size > MAX_PATH_PARTS) {
            fprintf(stderr, "which: error: Your path has more than %i directories.\n", MAX_PATH_PARTS);
            fprintf(stderr, "I never thought this would happen.\n");
            fprintf(stderr, "Head to https://da.gd/big-path to open an issue, and I'll fix it. <3\n");
            exit(-1);
        }
    }

    return 0;
}

static int which(Path *path, char *name, int all, int quiet) {
    int found_any = 0;

    for (size_t i = 0; i < path->size; i++) {
        int length = snprintf(NULL, 0, "%s/%s", path->parts[i], name);
        size_t size = (size_t)length + 1;
        char *file_path = malloc(size);
        snprintf(file_path, size, "%s/%s", path->parts[i], name);

        if (access(file_path, F_OK) != -1) {
            found_any = 1;
            if (!quiet) {
                puts(file_path);
            }
            if (!all) {
                break;
            }
        }

        free(file_path);
    }

    return found_any - 1;
}

int main(int argc, char **argv) {
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: which [-as] PROGRAM...");
        puts("Search the user's path for executables.");
        puts("-a    Print all instances of each program");
        puts("-s    Print nothing; return 0 if all programs are found, 1 otherwise");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc < 2) {
        bu_missing_argument(argv[0]);
        return 1;
    }

    int all = 0;
    int quiet = 0;
    int first_program = 0;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            first_program = i;
            break;
        }

        for (size_t j = 1; j < strlen(argv[i]); j++) {
            if (argv[i][j] == 'a') {
                all = 1;
            }
            if (argv[i][j] == 's') {
                quiet = 1;
            }
        }
    }

    Path path = {{0}, 0, 0};
    if (parse_path(&path) == -1) {
        fprintf(stderr, "which: environment variable PATH is not set\n");
        return 1;
    }

    int ret = 0;
    for (int i = first_program; i < argc; i++) {
        if (which(&path, argv[i], all, quiet) == -1) {
            ret = 1;
        }
    }

    return ret;
}
