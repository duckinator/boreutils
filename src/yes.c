#include <stdio.h>
#include <string.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s [STRING]\n\n", argv[0]);
        printf("Print a string repeatedly until killed.\n\n");
        printf("Options:\n");
        printf("  STRING    string to print (default: \"y\")\n");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    // Endlessly print all of the arguments.
    while(1) {
        // If we got no arguments, print "y\n" and go to the next iteration.
        if (argc < 2) {
            puts("y");
            continue;
        }

        // If we get here, we just dump the entirety of argv.
        for (int i = 1; i < argc - 1; i++) {
            printf("%s ", argv[i]);
        }
        printf("%s\n", argv[argc]);
    }
}
