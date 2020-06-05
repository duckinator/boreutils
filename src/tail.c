/**
 * NAME
 * ====
 *     tail - copy the first part of files
 *
 * SYNOPSIS
 * ========
 *     tail [-f] [-c NUMBER|-n NUMBER] [FILE]
 *     tail [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     For each FILE, read the last NUMBER of lines (-n) or bytes (-c) and
 *     print them to standard output.
 *
 *     If neither -c nor -n is specified, tail defaults to -n 10.
 *
 *     For any arguments named NUMBER:
 *     * It must be a decimal integer, optionally including a sign.
 *     * If it starts with a + sign, it's relative to the beginning of the file.
 *     * If it starts with a - sign, it's relative to the end of the file.
 *     * If it starts with neither a + nor - sign, it is treated as though
 *       it starts with a -.
 *
 *
 *     -f           "Follow" appended output after normal operation.
 *     -c NUMBER    The byte offset to start from.
 *     -n NUMBER    The line offset to start from.
 *     FILE         The file(s) to print.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/tail.html

static int dash_c = 0;
static int dash_n = 0;

static int copy_rest(FILE *stream) {
    char *line = NULL;
    size_t n = 0;
    ssize_t bytes_read = 1;
    while (bytes_read >= 0) {
        bytes_read = getline(&line, &n, stream);

        if (bytes_read == -1) {
            break;
        }

        if (bytes_read > 0 && line != NULL) {
            fputs(line, stdout);
        }
    }

    if (line != NULL) {
        free(line);
    }

    return 0;
}

static void skip_bytes(FILE *stream, int bytes) {
    for (int i = 0; i < bytes; i++) {
        int ret = fgetc(stream);
        if (ret == EOF) {
            return;
        }
    }
}

static void tail_bytes(FILE *stream, int bytes) {
    if (bytes > 0) { // skip <bytes> chars, print everything else
        skip_bytes(stream, bytes);
        copy_rest(stream);
    } else { // print last <bytes> chars
        bytes = -bytes;
        char *buf = malloc(sizeof(char) * ((size_t)bytes + 1));
        int idx = 0;

        while (1) {
            int ret = fgetc(stream);
            if (ret == EOF) {
                break;
            }
            buf[idx] = (char)ret;

            if (idx > bytes) {
                for (int i = 0; i < bytes; i++) {
                    buf[i] = buf[i + 1];
                }
                buf[bytes] = 0;
                idx = 0;
            }

            idx++;
        }

        buf[bytes] = 0;
        fputs(buf, stdout);
    }
}

static void skip_lines(FILE *stream, int lines) {
    char *line = NULL;
    size_t n = 0;
    ssize_t bytes_read = 0;
    for (int i = 0; (i < lines) && (bytes_read >= 0); i++) {
        bytes_read = getline(&line, &n, stream);

        if (bytes_read == -1) {
            break;
        }
    }

    if (line != NULL) {
        free(line);
    }

    if (bytes_read == -1) {
        perror("tail");
        exit(1);
    }
}

static void tail_lines(FILE *stream, int lines) {
    if (lines > 0) {
        skip_lines(stream, lines);
        copy_rest(stream);
        return;
    }

    lines = -lines;

    char **linebuf = malloc(sizeof(char*) * (size_t)(lines + 1));
    char *line = NULL;
    size_t n = 0;
    ssize_t bytes_read = 1;
    int line_idx = 0;
    while (bytes_read >= 0) {
        line = NULL;
        n = 0;
        bytes_read = getline(&line, &n, stream);

        if (bytes_read == -1) {
            break;
        }

        if (bytes_read > 0 && line != NULL) {
            linebuf[line_idx] = malloc(sizeof(char) * (strlen(line) + 1));
            strcpy(linebuf[line_idx], line);
            free(line);
            line_idx++;
        }
        if (line_idx > lines) {
            free(linebuf[0]);
            for (int i = 0; i < lines; i++) {
                linebuf[i] = linebuf[i + 1];
            }
            line_idx = lines;
        }
    }

    if (line != NULL) {
        free(line);
    }

    for (int i = 0; i < lines; i++) {
        fputs(linebuf[i], stdout);
    }
}

static void tail_stream(FILE *stream, int bytes, int lines) {
    if (dash_c) {
        tail_bytes(stream, bytes);
    } else {
        tail_lines(stream, lines);
    }
}

static void tail_file(char *path, int bytes, int lines) {
    FILE *stream = fopen(path, "r");

    if (stream == NULL) {
        perror(path);
        exit(1);
    }

    tail_stream(stream, bytes, lines);
}

int main(int argc, char **argv)
{
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: tail [-f] [-c NUMBER|-n NUMBER] [FILE]");
        puts("Print a file starting from the specified offset.");
        puts("");
        puts("-c NUMBER     Start from byte NUMBER.");
        puts("-n NUMBER     Start from line NUMBER.");
        puts("");
        puts("If neither -c nor -n are specified, defaults to -n 10.");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    int lines = -10;
    int bytes = 0;
    int follow = 1;

    // We use `i < argc - 1` because we access `argv[i + 1]` in the loop.
    for (int i = 1; i < argc - 1; i++) {
        if (strncmp(argv[i], "-c", 3) == 0) {
            dash_c = 1;
            if (argc <= i) {
                bu_invalid_argument(argv[0], "-c needs a number");
                return 1;
            }
            bytes = atoi(argv[i + 1]);
        }
        if (strncmp(argv[i], "-n", 3) == 0) {
            dash_n = 1;
            if (argc <= i) {
                bu_invalid_argument(argv[0], "-n needs a number");
                return 1;
            }
            if (argv[i + 1][0] == '-' || argv[i + 1][0] == '+') {
                lines = atoi(argv[i + 1]);
            } else {
                lines = -atoi(argv[i + 1]);
            }
        }
        if (strncmp(argv[i], "-f", 3) == 0) {
            follow = 1;
        }
    }

    if (dash_c && dash_n) {
        bu_invalid_argument(argv[0], "-n and -c can't be used together");
        return 1;
    }

    if (((dash_c || dash_n) && (argc > 4)) || (!dash_c && !dash_n && (argc > 2))) {
        bu_extra_argument(argv[0]);
        return 1;
    }

    int has_file = ((dash_n || dash_c) && (argc == 4)) || (!dash_n && !dash_c && argc == 2) ? 1 : 0;

    if (!has_file || (strncmp(argv[argc - 1], "-", 3) == 0)) {
        tail_stream(stdin, bytes, lines);
    } else {
        tail_file(argv[argc - 1], bytes, lines);
    }

    // TODO: Handle -f.

    return 0;
}
