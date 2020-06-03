/**
 * NAME
 * ====
 *     echo - print a line of text
 *
 * SYNOPSIS
 * ========
 *     echo ARGS...
 *
 * DESCRIPTION
 * ===========
 *     Prints all arguments, separated by spaces, followed by a newline.
 *
 *     Notably, this utility does NOT accept --help or --version.
 *
 *     Due to the compatibility problems between the two prevailing varieties
 *     of the `echo` utility, it's STRONGLY recommended to use `printf` instead.
 */


#include <stdio.h>

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/echo.html

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        fputs(argv[i], stdout);
        if (i < (argc - 1)) {
            fputs(" ", stdout);
        }
    }
    puts("");
    return 0;
}
