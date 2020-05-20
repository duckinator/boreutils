#include <stdio.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/false.html

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s\n\n", argv[0]);
        printf("Do nothing and fail.\n");
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    return 1;
}
