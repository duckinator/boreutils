/**
 * NAME
 * ====
 *     mv - move (rename) files
 *
 * SYNOPSIS
 * ========
 *     mv [-if] source_file target_file
 *     mv [-if] source_file... target_dir
 *     mv [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Move files.
 *
 *     -i           Prompt when a file would be overwritten.
 *     -f           Don't prompt when a file would be overwritten.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */

#include <stdio.h>
#include <unistd.h>
#include "boreutils.h"

static int prompt(char *dest) {
    char ret[2] = "-";
    fprintf(stderr, "mv: overwrite '%s'? [y/N] ", dest);
    fgets(ret, 2, stdin);

    if (ret[0] == 'y' || ret[0] == 'Y') {
        return 1;
    }
    return 0;
}

static int would_overwrite(char *src, char *dest) {
    (void)src;
    return access(dest, F_OK) + 1;
}

int main(int argc, char **argv) {
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: mv [-if] source_file target_file");
        puts("       mv [-if] source_file... target_dir");
        puts("Move (rename) files.");
        puts("-i           Prompt when a file would be overwritten.");
        puts("-f           Don't prompt when a file would be overwritten.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    int ret = 0;
    int should_prompt = 0;
    char *dest = argv[argc - 1];

    int i = 1;
    for (; i < argc; i++) {
        if (argv[i][0] != '-') {
            break;
        }

        for (size_t j = 1; j < strlen(argv[i]); j++) {
            if (argv[i][j] == 'f') {
                should_prompt = 0;
            } else if (argv[i][j] == 'i') {
                should_prompt = 1;
            } else {
                char arg[3] = {'-', argv[i][j], 0};
                bu_invalid_argument(argv[0], arg);
            }
        }
    }

    for (; i < argc - 1; i++) {
        if (should_prompt && would_overwrite(argv[i], dest)) {
            if (prompt(dest) == 0) {
                continue;
            }
        }

        if (rename(argv[i], dest) == -1) {
            perror("mv");
            ret = 1;
        }
    }
    return ret;
}
