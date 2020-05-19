#ifndef BOREUTILS_H
#define BOREUTILS_H

#include <stdio.h>
#include <string.h>

// BU_PATH_BUFSIZE is 8KB which is, in practice, larger than any PATH_MAX
// value I've encountered.
//
// Given that path names can be longer than PATH_MAX anyway, it seemed
// easier to just hard-code it instead of adding yet another dependency.
#define BU_PATH_BUFSIZE 8192

static const char *BOREUTILS_VERSION = "0.0.0b1";

int has_arg(int argc, char **argv, char *search);
void bu_missing_argument(char *name);
void bu_extra_argument(char *name);

int bu_handle_version(int argc, char **argv);


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


void bu_missing_argument(char *name) {
    fprintf(stderr, "%s: Missing argument\nSee '%s --help' for usage information.\n", name, name);
}


void bu_extra_argument(char *name) {
    fprintf(stderr, "%s: Extra argument\nSee '%s --help' for usage information.\n", name, name);
}


int bu_handle_version(int argc, char **argv) {
    if (has_arg(argc, argv, "--version")) {
        printf("%s (Boreutils) %s\n", argv[0], BOREUTILS_VERSION);
        return 1;
    }

    return 0;
}

#endif
