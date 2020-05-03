#include <stdio.h>
#include <unistd.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s\n\n", argv[0]);
        printf("Print effective userid.\n");
        return 0;
    }

    char *ret = getlogin();

    if (ret == NULL) {
        perror(argv[0]);
        return 1;
    }

    puts(ret);

    return 0;
}
