#include <stdio.h>
#include <unistd.h>
#include "boreutils.h"

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s\n\n", argv[0]);
        puts("Print the name of the current working directory.");
        return 0;
    }

    char buf[4096];
    char *ret = getcwd(buf, sizeof(buf));

    if (ret == NULL) {
        // TODO: Actually handle cases where URLs are >4k.
        perror(argv[0]);
        return 1;
    }

    puts(ret);

    return 0;
}
