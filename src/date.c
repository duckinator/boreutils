#include <stdio.h>
#include <time.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/date.html

static int set_time(char *date, char **argv) {
    size_t len = strlen(date);
    struct tm tmp;
    char *ret;

    memset(&tmp, 0, sizeof(tmp));

    if (len == 12) {
        // MMDDhhmmCCYY
        ret = strptime(date, "%m%d%H%M%Y", &tmp);
    } else if (len == 10) {
        // MMDDhhmmYY
        ret = strptime(date, "%m%d%H%M%y", &tmp);
    } else if (len == 8) {
        // MMDDhhmm
        ret = strptime(date, "%m%d%H%M", &tmp);
    } else {
        fprintf(stderr, "Invalid date string.\nSee '%s --help' for usage information.\n", argv[0]);
        return 1;
    }

    if (ret == NULL) {
        fprintf(stderr, "Somehow tried using invalid date string?\nThis error should never happen.\n");
        return 1;
    }

    if (ret[0] != '\0') {
        fprintf(stderr, "Somehow gave extra data to strptime()?\nThis error should never happen.\n");
        return 1;
    }


    struct timespec ts;
    time_t tval = mktime(&tmp);
    if (tval == -1) {
        fprintf(stderr, "Error calling mktime().\n");
        return 1;
    }
    ts.tv_sec = tval;
    ts.tv_nsec = 0;

    if (clock_settime(CLOCK_REALTIME, &ts) == 0) {
        perror(argv[0]);
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s [-u] [+FORMAT]\n", argv[0]);
        printf("       %s [-u] MMDDhhmm[[CC]YY]\n", argv[0]);
        printf("Print the current time in the specified FORMAT, or set the date.\n");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    char *format_str = NULL;
    char *date_str = NULL;
    int dash_u = 0;
    int should_set_time = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0) {
            dash_u = 1;
        } else if (argv[i][0] == '+') {
            format_str = argv[i];
            format_str++; // Drop the leading +.
        } else {
            should_set_time = 1;
            date_str = argv[i];
        }
    }

    if (should_set_time) {
        return set_time(date_str, argv);
    }

    if (format_str == NULL) {
        format_str = "%a %b %e %H:%M:%S %Z %Y";
    }

    // If you need a >200 character long output, lmk and I'll fix this.
    char *buf[200];

    time_t t;
    struct tm *current_time;

    t = time(NULL);
    if (dash_u) {
        current_time = gmtime(&t);
    } else {
        current_time = localtime(&t);
    }

    if (current_time == NULL) {
        perror(argv[0]);
        return 1;
    }

    if (strftime((char*)buf, sizeof(buf), format_str, current_time)) {
        puts((char*)buf);
    }

    return 0;
}
