/**
 * NAME
 * ====
 *     date - print or set the system date and time
 *
 * SYNOPSIS
 * ========
 *     date [-u] [+FORMAT]
 *     date [-u] MMDDhhmm[[CC]YY]
 *     date [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     In the first usage, the date utility prints the current date and time
 *     in the specified FORMAT.
 *
 *     In the second usage, the date utility attempts to set the date and time.
 *
 *     -u       Use UTC time.
 *
 *     +FORMAT  If FORMAT is specified, each conversion specifier will be
 *              replaced with the appropriate value as specified below.
 *              All other characters will be copied without change.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 *
 * Conversion Specifiers
 * ---------------------
 *     See strftime.
 */


#include <stdio.h>
#include <time.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/date.html

static int set_time(char *date, char **argv) {
    size_t len = strlen(date);
    struct tm tmp;
    char *ret = NULL;

    // For determining the current year.
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);

    memset(&tmp, 0, sizeof(tmp));

    if (len == 12) {
        // MMDDhhmmCCYY
        ret = strptime(date, "%m%d%H%M%Y", &tmp);
    } else if (len == 10) {
        // MMDDhhmmYY
        ret = strptime(date, "%m%d%H%M%y", &tmp);
    } else if (len == 8) {
        // MMDDhhmm
        // Set the year, since it's not specified.
        tmp.tm_year = local->tm_year;
        ret = strptime(date, "%m%d%H%M", &tmp);
    }

    if (ret == NULL) {
        fputs("Invalid date string\nSee 'date --help' for usage information.\n", stderr);
        return 1;
    }

    if (ret[0] != '\0') {
        fputs("Somehow gave extra data to strptime()? (This shouldn't happen!)\n", stderr);
        return 1;
    }


    struct timespec ts;

    // Convert from `struct tm` to `time_t`.
    time_t tval = mktime(&tmp);
    if (tval == -1) {
        fputs("Error calling mktime().\n", stderr);
        return 1;
    }

    // Convert tval to local time.
    struct tm *local_ts = localtime(&tval);
    tval = mktime(local_ts);
    if (tval == -1) {
        fputs("Error calling mktime(). (Second call.)\n", stderr);
        return 1;
    }

    ts.tv_sec = tval;
    ts.tv_nsec = 0;

    if (clock_settime(CLOCK_REALTIME, &ts) == -1) {
        perror(argv[0]);
        return 1;
    }

    return 0;
}

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: date [-u] [+FORMAT]");
        puts("       date [-u] MMDDhhmm[[CC]YY");
        puts("Print the current time in the specified FORMAT, or set the date.");
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
