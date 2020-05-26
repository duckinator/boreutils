#include <stdio.h>
#include <stdlib.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/cal.html

// afaict, this is the max full _or_ partial weeks in a month.
#define MAX_WEEKS_IN_MONTH 6

#define MONTH_BUF_WEEK 21 // len('DD ') * 7, last is \n instead of space.
#define MONTH_BUF (MONTH_BUF_WEEK * (MAX_WEEKS_IN_MONTH + 2 /* header lines */))

// Hard-code September 1752, the month POSIX-compatible `cal` treats as
// the transition between the Julian and Gregorian calendars.
static char sep1752[MONTH_BUF] = "\
   September 1752   \n\
Su Mo Tu We Th Fr Sa\n\
       1  2 14 15 16\n\
17 18 19 20 21 22 23\n\
24 25 26 27 28 29 30\n\
                    \n\
                    \n\
                    ";

static char g_month_names[13][20] = {
    {0},
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
};

static char g_days_in_month[2][13] = {
    // Non-leap years.
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    // Leap years.
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

static int g_is_leap_year(int year) {
    return (year % 4 == 0) && !( (year % 100 == 0) && (year % 400 != 0) );
}

static long g_daystotal(int y, int m, int d) {
    int days = d;
    for (int year = 1; year <= y; year++)
    {
        int max_month = ( year < y ? 12 : m-1 );
        int leap = g_is_leap_year(year);

        for (int month = 0; month < max_month; month++) {
            days += g_days_in_month[leap][month];
        }
    }
    return days;
}

static int g_day_of_week(int y, int m, int d) {
    return (g_daystotal(y, m, d) % 7) + 1;
}

static char *g_build_month(char *buf, int y, int m) {
    char *week_str = buf + (MONTH_BUF_WEEK * 2);
    int first_dow = g_day_of_week(y, m, 1);
    int last_date = g_days_in_month[g_is_leap_year(y)][m - 1];
    char *month = g_month_names[m];
    size_t month_len = strlen(month);
    size_t offset = (21 /* str size */ - 5 /* year */ - month_len) / 2;

    // Fill the first line with spaces.
    memset(buf, ' ', MONTH_BUF_WEEK);
    // Skip <offset> spaces, then copy in the month.
    (void)strncpy(buf + offset, month, month_len);
    // Copy in the year.
    snprintf(buf + offset + month_len, 6, " %i", y);
    // Note: Replace the null byte added by snprintf() with a space.
    buf[offset + month_len + 5] = ' ';

    buf[MONTH_BUF_WEEK - 1] = '\n';

    (void)strncpy(buf + MONTH_BUF_WEEK, "Su Mo Tu We Th Fr Sa\n", MONTH_BUF_WEEK);

    char date = 1;
    size_t last_row = 2; // headers
    for (size_t w = 0; w < MAX_WEEKS_IN_MONTH; w++, week_str += MONTH_BUF_WEEK) {
        if (date > last_date) {
            break;
        }

        last_row++;

        char *day_str = week_str;
        // += 3 because 'DD '/'DD<newline>'
        for (int dow = 1; dow <= 7; dow++, day_str += 3) {
            if (date > last_date) {
                break;
            }
            if ((date == 1) && (dow < first_dow)) {
                day_str[0] = ' ';
                day_str[1] = ' ';
                day_str[2] = ' ';
                continue;
            }

            // Note: snprintf() does add a null byte, _but_ this will be
            //       overwritten by assigning a newline or space below.
            snprintf(day_str, 3, "%2d", date);

            if (dow == 7) {
                day_str[2] = '\n';
            } else {
                day_str[2] = ' ';
            }
            date++;
        }
    }

    buf[MONTH_BUF_WEEK * last_row - 1] = 0;

    return buf;
}

static char *get_month(char *buf, size_t bufsize, int y, int m) {
    if (y <= 1752 && m < 9) {
        // Julian calendar
        strcpy(buf, "TODO: Julian calendar for dates before September 1752.\n");
        return buf;
    } else if (y == 1752 && m == 9) {
        // Hard-coded September 1752 because it's Complicated(TM).
        strncpy(buf, sep1752, bufsize);
        return buf;
    } else {
        // Gregorian calendar.
        return g_build_month(buf, y, m);
    }
}

static void print_month(int y, int m) {
    char buf[MONTH_BUF] = {0};
    puts(get_month((char*)buf, MONTH_BUF, y, m));
}

static void print_year(int y) {
    for (int m = 1; m <= 12; m++) {
        print_month(y, m);
        puts("");
    }
}

int main(int argc, char **argv) {
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        printf("Usage: %s\n\n", argv[0]);
        printf("Do nothing and succeed.\n");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    int y = 0;
    int m = 0;

    if (argc > 3) {
        bu_extra_argument(argv[0]);
        return 1;
    }

    if (argc == 3) {
        y = (int)strtol(argv[2], (char**)NULL, 10);
        m = (int)strtol(argv[1], (char**)NULL, 10);
    }

    if (argc == 2) {
        y = (int)strtol(argv[1], (char**)NULL, 10);
    }

    if (y == 0 && m == 0) {
        // TODO: Don't hard-code this.
        y = 2020;
        m = 5;
    }

    if (m) {
        print_month(y, m);
    } else {
        print_year(y);
    }

    return 0;
}
