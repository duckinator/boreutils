/**
 * NAME
 * ====
 *     cal - print a calendar
 *
 * SYNOPSIS
 * ========
 *     cal
 *     cal YEAR
 *     cal MONTH YEAR
 *
 * DESCRIPTION
 * ===========
 *     In the first invocation, prints a calendar for the current month and year.
 *
 *     In the second invocation, prints a calendar for the specified year.
 *
 *     In the third invocation, prints a calendar for the specified month of
 *     the specified year.
 *
 *     Dates starting with September 14 1752 use the Gregorian calendar.
 *     Dates prior to that date use the Julian calendar.
 *
 *     MONTH        The month, as a number from 1 (January) to 12 (December).
 *     YEAR         The year, as a number.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */



#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/cal.html

// afaict, this is the max full _or_ partial weeks in a month.
#define MAX_WEEKS_IN_MONTH 6

#define MONTH_BUF_WEEK 21 // len('DD ') * 7, last is \n instead of space.
#define MONTH_BUF (MONTH_BUF_WEEK * (MAX_WEEKS_IN_MONTH + 2 /* header lines */))

// The month names for the calendar. The 1-indexed.
static char month_names[13][20] = {
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

// (Julian calendar) Given a month buffer, year, and month, render a calendar.
static char *j_build_month(char *buf, int y, int m) {
    (void)y;
    (void)m;
    strcpy(buf, "TODO: Julian calendar for dates before September 1752.\n");
    return buf;
}

// (Julian->Gregorian transition)
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

// (Gregorian calendar)
// Stores the number of days in each month (indexed as 0-11 for Jan-Dec),
// for both leap and non-leap years.
static char g_days_in_month[2][13] = {
    // Non-leap years.
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    // Leap years.
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
};

// (Gregorian calendar)
// Given a year, return 1 if it's a leap year, 0 otherwise.
static int g_is_leap_year(int year) {
    return (year % 4 == 0) && !( (year % 100 == 0) && (year % 400 != 0) );
}

// (Gregorian calendar)
// Given a year, month, and day, determine the number of days since Jan 1, 1.
// This is based on the "Rata Die" method:
// https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Rata_Die
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

// (Gregorian calendar)
// Given a year, month, and day, determine the day of the week.
static int g_day_of_week(int y, int m, int d) {
    return (g_daystotal(y, m, d) % 7) + 1;
}

// (Gregorian calendar)
// Given a month buffer, year, and month, render a calendar.
static char *g_build_month(char *buf, int y, int m) {
    char *week_str = buf + (MONTH_BUF_WEEK * 2);
    int first_dow = g_day_of_week(y, m, 1);
    int last_date = g_days_in_month[g_is_leap_year(y)][m - 1];
    char *month = month_names[m];
    size_t month_len = strlen(month);
    size_t offset = (21 /* str size */ - 5 /* year */ - month_len) / 2;

    // Fill the buffer with spaces.
    memset(buf, ' ', MONTH_BUF - 1);

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
        return j_build_month(buf, y, m);
    } else if (y == 1752 && m == 9) {
        // Hard-coded September 1752 because it's the
        // Complicated Transition Period(TM).
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
        puts("Usage: cal [[MONTH] YEAR]");
        puts("Prints a calendar for the specified timeframe.\n");
        puts("If MONTH and YEAR are provided, prints the calendar for the specified month.");
        puts("If only YEAR is provided, prints the calendar for all of YEAR.");
        puts("Otherwise, prints the calendar for the current month and year.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    if (argc > 3) {
        bu_extra_argument(argv[0]);
        return 1;
    }

    int year = 0;
    int month = 0;

    if (argc == 3) {
        year = (int)strtol(argv[2], (char**)NULL, 10);
        month = (int)strtol(argv[1], (char**)NULL, 10);
    }

    if (argc == 2) {
        year = (int)strtol(argv[1], (char**)NULL, 10);
    }

    if (year == 0 && month == 0) {
        time_t current_time = time(NULL);
        struct tm *ltime = localtime(&current_time);

        // Month is 0-11, add 1 to get 1-12.
        month = ltime->tm_mon + 1;
        // Year is years since 1900, add 1900 for actual year.
        year = ltime->tm_year + 1900;
    }

    if (month) {
        print_month(year, month);
    } else {
        print_year(year);
    }

    return 0;
}
