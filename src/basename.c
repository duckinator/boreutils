#include <stdio.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/basename.html

static int consists_entirely_of(char *string, char value) {
    for (size_t i = 0; i < strlen(string); i++) {
        if (string[i] != value) {
            return 0;
        }
    }
    return 1;
}

static void truncate_trailing_slashes(char *string) {
    size_t pos = strlen(string);
    for (size_t i = 0; i < strlen(string); i++) {
        if (string[pos] == '/') {
            // If the current character is a slash, truncate the string.
            string[pos] = '\0';
        } else {
            // If the current character is _not_ a slash, break immediately.
            break;
        }
        pos--;
    }
}
/*
static char *after_last_slash(char *string) {
    size_t pos = strlen(string);
    for (size_t i = 0; i < strlen(string); i++) {
        pos--;
        if (string[i] == '/') {
            return (string + pos);
        }
    }
    return string;
}

static char *remove_suffix(char *string, char *suffix) {
    if (suffix) { return string; }
    return string;
}
*/
int main(int argc, char **argv) {
    if (argc < 2) {
        bu_missing_argument(argv[0]);
        return 1;
    }

    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s FILE [SUFFIX]\n\n", argv[0]);
        puts("Strip the directory and SUFFIX from FILE.");
        return 1;
    }

    char *string = argv[1];

    // 1. If string is NULL, POSIX.1-2017 allows to return either '.' or
    //    a null string. We go with a null string.
    if ((argc < 2) || (argv[0] == NULL)) {
        puts("");
        return 0;
    }

    // 2. If string is "//", it is implementation-defined whether steps 3-6
    //    are skipped. Thus, we have no code for step 2.

    // 3. If string consists entirely of slashes, return a single slash.
    if (consists_entirely_of(string, '/')) {
        puts("/");
        return 0;
    }

    // 4. Remove trailing slashes.
    truncate_trailing_slashes(string);

    // 5. If there are more slashes, everything up to the last one.
    //string = after_last_slash(string);

    // 6. If a suffix is provided and not identical to the remaining string,
    //    remove it. Otherwise, do nothing.
    if (argc > 2) {
        //string = remove_suffix(string, argv[2]);
    }

    puts(string);
    return 0;
}
