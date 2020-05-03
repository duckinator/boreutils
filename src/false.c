#include <stdio.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s\n\n", argv[0]);
        printf("Do nothing and fail.\n");
    }

    return 1;
}
