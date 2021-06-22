#ifndef BOREUTILS_H
#define BOREUTILS_H

#include <stdio.h>
#include <string.h>

#if defined(__has_feature)
#   if __has_feature(memory_sanitizer)
// If memory sanitizer feature is available.
#define NO_MEM_SANITIZE __attribute__((no_sanitize("memory")))
#   else
// If memory sanitizer feature is not available.
#define NO_MEM_SANITIZE
#   endif
#endif

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
void bu_invalid_argument(char *name, char *arg);

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
    // <name>: Missing argument
    fputs(name, stderr);
    fputs(": Missing argument\n", stderr);

    // See '<name> --help' for usage information.
    fputs("See '", stderr);
    fputs(name, stderr);
    fputs(" --help' for usage information.\n", stderr);
}


void bu_extra_argument(char *name) {
    // <name>: Extra argument
    fputs(name, stderr);
    fputs(": Extra argument\n", stderr);
    // See '<name> --help' for usage information.
    fputs("See '", stderr);
    fputs(name, stderr);
    fputs(" --help' for usage information.\n", stderr);
}


void bu_invalid_argument(char *name, char *arg) {
    // <name>: Extra argument
    fputs(name, stderr);
    fputs(": Invalid argument: ", stderr);
    fputs(arg, stderr);
    fputs("\n", stderr);
    // See '<name> --help' for usage information.
    fputs("See '", stderr);
    fputs(name, stderr);
    fputs(" --help' for usage information.\n", stderr);
}


int bu_handle_version(int argc, char **argv) {
    if (has_arg(argc, argv, "--version")) {
        fputs(argv[0], stdout);
        fputs(" (Boreutils) ", stdout);
        puts(BOREUTILS_VERSION);
        return 1;
    }

    return 0;
}

#endif
