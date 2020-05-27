/**
 * NAME
 * ====
 *     dirname - strip last component of a file path
 *
 * SYNOPSIS
 * ========
 *     dirname PATH
 *     dirname [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Strips the last component of a file path.
 *     The result is printed to standard output.
 *
 *     PATH         The file path.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/dirname.html

static int consists_entirely_of(char *string, char value) {
    for (size_t i = 0; i < strlen(string); i++) {
        if (string[i] != value) {
            return 0;
        }
    }
    return 1;
}

static void truncate_trailing_slashes(char *string) {
    size_t len = strlen(string);
    size_t pos = len;
    for (size_t i = 0; i <= len; i++) {
        pos--;
        if (string[pos] == '/') {
            // If the current character is a slash, truncate the string.
            string[pos] = '\0';
        } else {
            // If the current character is _not_ a slash, break immediately.
            break;
        }
    }
}

static void truncate_trailing_non_slashes(char *string) {
    size_t len = strlen(string);
    size_t pos = len;
    for (size_t i = 0; i <= len; i++) {
        pos--;
        if (string[pos] == '/') {
            // If the current character is a slash, break immediately.
            break;
        } else {
            // If the current character is _not_ a slash, truncate the string.
            string[pos] = '\0';
        }
    }
}

static int has_slash(char *string) {
    size_t len = strlen(string);
    for (size_t i = 0; i < len; i++) {
        if (string[i] == '/') {
            // Found a slash.
            return 1;
        }
    }
    // No slash found.
    return 0;
}

int main(int argc, char **argv) {
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: dirname PATH");
        puts("Strip the last component of a file path.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc < 2) {
        bu_missing_argument(argv[0]);
        return 1;
    }

    if (argc > 2) {
        bu_extra_argument(argv[0]);
        return 1;
    }

    char *string = argv[1];

    // 1. If string is "//", return "/".
    if (strncmp("//", string, 3) == 0) {
        puts("/");
        return 0;
    }

    // 2. If string consists entirely of slashes, return a single slash.
    if (consists_entirely_of(string, '/')) {
        puts("/");
        return 0;
    }

    // 3. Remove trailing slashes.
    truncate_trailing_slashes(string);

    // 4. If there are no slashes, return ".".
    if (!has_slash(string)) {
        puts(".");
        return 0;
    }

    // 5. If there are any trailing non-<slash> characters, remove them.
    truncate_trailing_non_slashes(string);

    // 6. If the remaining string is "//", it is implementation defined
    //    whether to skip the remaining steps.
    //    We choose to _not_ skip them, so there is no code for this step.

    // 7. Remove trailing slashes again.
    truncate_trailing_slashes(string);

    // 8. If the remaining string is empty, return "/".
    if (strlen(string) == 0) {
        puts("/");
        return 0;
    }

    puts(string);
    return 0;
}
