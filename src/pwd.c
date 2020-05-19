#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/pwd.html

int main(int argc, char **argv)
{
    bool dash_p = false;

    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s [-L|-P]\n\n", argv[0]);
        puts("Print the name of the current working directory.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    // Oh no.
    // "If both -L and -P are specified, the last one shall apply."
    // Why, POSIX. WHY. ;~;
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-P", 3) == 0) {
            dash_p = true;
        } else if (strncmp(argv[i], "-L", 3) == 0) {
            dash_p = false;
        } else {
            // Got something that's not -L or -P?
            bu_extra_argument(argv[0]);
            return 1;
        }
    }

    char buf[BU_PATH_BUFSIZE];
    char *ret = getcwd(buf, sizeof(buf));

    if (dash_p) {
        char buf2[BU_PATH_BUFSIZE];
        char *ret2 = realpath(ret, buf2);
        ret = ret2;
    }

    if (ret == NULL) {
        perror(argv[0]);
        return 1;
    }

    puts(ret);

    return 0;
}
