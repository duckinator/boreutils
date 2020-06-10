/**
 * NAME
 * ====
 *     uname - print system information
 *
 * SYNOPSIS
 * ========
 *     uname [-amnrsv]
 *     uname [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Prints the requested system information. If no options provided, defaults
 *     to -s.
 *
 *     -a   Equivalent to -mnrsv.
 *     -m   Print hardware type.
 *     -n   Print network hostname.
 *     -r   Print operating system release.
 *     -s   Print the kernel name.
 *     -v   Print the version of the installed operating system release.
 */


#include <stdio.h>
#include <sys/utsname.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: uname [-amnrsv]");
        puts("Prints system information.");
        puts("-a   Equivalent to -mnrsv");
        puts("-m   Print hardware type");
        puts("-n   Print network hostname");
        puts("-r   Print operating system release");
        puts("-s   Print the kernel name");
        puts("-v   Print the version of the installed operating system release");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    int dash_m = 0;
    int dash_n = 0;
    int dash_r = 0;
    int dash_s = 0;
    int dash_v = 0;

    if (argc == 1) {
        // default is equivalent to `uname -s`
        dash_s = 1;
    }

    int i = 1;
    for (; i < argc; i++) {
        if (argv[i][0] != '-') { // All arguments should be flags.
            bu_invalid_argument(argv[0], argv[i]);
            return 1;
        }

        for (size_t j = 1; j < strlen(argv[i]); j++) {
            char flag = argv[i][j];
            if (flag == 'a') {
                dash_m = 1;
                dash_n = 1;
                dash_r = 1;
                dash_s = 1;
                dash_v = 1;
            }
            else if (flag == 'm') { dash_m = 1; }
            else if (flag == 'n') { dash_n = 1; }
            else if (flag == 'r') { dash_r = 1; }
            else if (flag == 's') { dash_s = 1; }
            else if (flag == 'v') { dash_v = 1; }
            else {
                char bad_flag_msg[16] = "unknown flag: -X";
                bad_flag_msg[15] = flag;
                bu_invalid_argument(argv[0], bad_flag_msg);
                return 1;
            }
        }
    }

    int remaining = dash_m + dash_n + dash_r + dash_s + dash_v;

    struct utsname name;

    if (uname(&name) == -1) {
        perror(argv[0]);
        return 1;
    }

    if (dash_s) {
        fputs(name.sysname, stdout);
        if (remaining > 1) {
            fputc(' ', stdout);
        }
        remaining--;
    }

    if (dash_n) {
        fputs(name.nodename, stdout);
        if (remaining > 1) {
            fputc(' ', stdout);
        }
        remaining--;
    }

    if (dash_r) {
        fputs(name.release, stdout);
        if (remaining > 1) {
            fputc(' ', stdout);
        }
        remaining--;
    }

    if (dash_v) {
        fputs(name.version, stdout);
        if (remaining > 1) {
            fputc(' ', stdout);
        }
        remaining--;
    }

    if (dash_m) {
        fputs(name.machine, stdout);
        if (remaining > 1) {
            fputc(' ', stdout);
        }
        remaining--;
    }

    puts("");

    return 0;
}
