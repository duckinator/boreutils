#ifndef BOREUTILS_H
#define BOREUTILS_H

#include <string.h>

int has_arg(int argc, char **argv, char *search);
void bu_missing_operand(char *name);


// FIXME: Having this in a header is definitely a hack.
int has_arg(int argc, char **argv, char *search)
{
    for (int idx = 1; idx < argc; idx++) {
        if (strcmp(argv[idx], search) == 0) {
            return 1;
        }
    }

    return 0;
}


void bu_missing_operand(char *name) {
    printf("%s: Missing operand\nTry '%s --help' for more information.\n", name, name);
}

#endif
