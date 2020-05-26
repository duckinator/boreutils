/**
 * NAME
 * ====
 *     pwd - prints the current working directory
 *
 * SYNOPSIS
 * ========
 *     pwd [-L|-P]
 *     pwd [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Prints the full filename of the current working directory.
 *
 *     -L   Use the PWD environment variable, even if it contains symlinks.
 *     -P   Avoid all symlinks.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/pwd.html

static int valid_pwd(char *cwd, char *env_pwd) {
    char rp_pwd_buf[BU_PATH_BUFSIZE];
    char *rp_pwd = realpath(env_pwd, rp_pwd_buf);

    size_t env_pwd_len = strlen(env_pwd);

    // If realpath() disapproves of $PWD, return 0.
    if (rp_pwd == NULL) {
        return 0;
    }

    // If it's not an absolute path, return 0.
    if (env_pwd[0] != '/') {
        return 0;
    }

    // If PWD doesn't point to the cwd, return 0.
    if (strcmp(cwd, rp_pwd) != 0) {
        return 0;
    }

    // If it contains a dot-dot segment, return 0.
    char *tmp = env_pwd + env_pwd_len - 5;
    if ((strstr(env_pwd, "/../") != NULL) || (strncmp(tmp, "/..", 4) == 0)) {
        return 0;
    }

    // If it contains a dot segment, return 0.
    tmp = env_pwd + env_pwd_len - 2;
    if ((strstr(env_pwd, "/./") != NULL) || (strncmp(tmp, "/.", 4) == 0)) {
        return 0;
    }

    return 1;
}

int main(int argc, char **argv)
{
    int dash_p = 0;

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
            dash_p = 1;
        } else if (strncmp(argv[i], "-L", 3) == 0) {
            dash_p = 0;
        } else {
            // Got something that's not -L or -P?
            bu_extra_argument(argv[0]);
            return 1;
        }
    }

    char buf[BU_PATH_BUFSIZE];
    char *env_pwd = getenv("PWD");
    char *cwd = getcwd(buf, sizeof(buf));
    char *ret = NULL;

    if (!dash_p && (env_pwd != NULL) && (env_pwd[0] != '\0') && valid_pwd(cwd, env_pwd)) {
        ret = env_pwd;
    } else {
        ret = cwd;
    }

    if (ret == NULL) {
        perror(argv[0]);
        return 1;
    }

    puts(ret);

    return 0;
}
