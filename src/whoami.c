#include <stdio.h>
#include <unistd.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s\n\n", argv[0]);
        printf("Print effective userid.\n");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc > 1) {
        bu_extra_argument(argv[0]);
        return 1;
    }

    char *ret = getlogin();

    if (ret == NULL) {
        perror(argv[0]);
        return 1;
    }

    puts(ret);

    return 0;
}
