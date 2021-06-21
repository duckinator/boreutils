/**
 * NAME
 * ====
 *     env - set or display the environment for command invocation
 *
 * SYNOPSIS
 * ========
 *     env [-i] [name=value]... [utility [argument...]]
 *     env [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     If a utility is specified, the environment is modified and then the
 *     utility is ran with the specified arguments.
 *
 *     If a utility is not specified, the environment is modified, and then
 *     env will print the contents of the environment.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "boreutils.h"

extern char **environ;

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/env.html

static int bu_clearenv(void) {
    /// clearenv() does not exist on all platforms, and setting the
    /// environ pointer directly can break things on some platforms.
    ///
    /// So, for now, we implement it in terms of unsetenv().
    while (environ[0]) {
        char *end = strchr(environ[0], '=');
        size_t len = (size_t)(end - environ[0]);
        char *name = malloc(sizeof(char) * (len + 1));
        strncpy(name, environ[0], len + 1);
        name[len] = 0;
        unsetenv(name);
        free(name);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: env [-i] [name=value]... [utility [argument...]]");
        puts("If a utility is specified, the environment is modified and the utility is ran.");
        puts("Otherwise, the environment is modified and then displayed.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    int i = 1; // argv[0] is the path to `env`, so we want to skip it.
    if (argc > 1 && strncmp(argv[1], "-i", 3) == 0) {
        i++;
        bu_clearenv();
    }

    for (; i < argc; i++) {
        if (!strchr(argv[i], '=')) {
            break;
        }
        putenv(argv[i]);
    }

    if (i < argc) {
        execvp(argv[i], argv + i);
        perror(argv[i]);
        return 1;
    } else {
        for (size_t j = 0; environ[j]; j++) {
            puts(environ[j]);
        }
    }

    return 0;
}
