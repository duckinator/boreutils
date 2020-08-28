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

#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
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

// If dest is a directory, returns `<dest>/<basename of src>`.
// Otherwise, returns `<dest>`.
static char *normalize_dest(char *src, char *dest) {
    struct stat statbuf;
    if (stat(dest, &statbuf) != 0) {
        // `dest` doesn't exist.
        return dest;
    }

    if (!S_ISDIR(statbuf.st_mode)) {
        // `dest` is a file (not a directory).
        return dest;
    }

    // Beyond this point, `dest` is known to be a directory.
    // This means we need `<dest>/<basename of src>`.

    // First, get the basename of `src`.
    char *base_name = basename(src);
    size_t base_name_size = strlen(base_name);

    size_t dest_size = strlen(dest);
    // <dest size> + <1 for slash> + <basename size> + <1 for null terminator>
    char *full_dest = malloc(sizeof(char) * (dest_size + 1 + base_name_size + 1));
    memset(full_dest, 0, dest_size + 1 + base_name_size + 1);

    strncpy(full_dest, dest, dest_size + 1);
    if (dest[dest_size - 1] != '/') {
        full_dest[dest_size] = '/';
        strncpy(full_dest + dest_size + 1, base_name, base_name_size + 1);
    } else {
        strncpy(full_dest + dest_size, base_name, base_name_size + 1);
    }

    return full_dest;
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
        char *normalized_dest = normalize_dest(argv[i], dest);
        if (should_prompt && access(normalized_dest, F_OK) == 0) {
            if (prompt(normalized_dest) == 0) {
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
