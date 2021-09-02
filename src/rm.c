/**
 * NAME
 * ====
 *     rm - remove directory entries
 *
 * SYNOPSIS
 * ========
 *     rm [-iRr] file...
 *     rm -f [-iRr] [file...]
 *
 * DESCRIPTION
 * ===========
 *     Remove the files or directories specified by file.
 *
 *     -f           Don't prompt for confirmation.
 *                  Don't print errors or modify exit status for missing arguments.
 *                  Don't print errors or modify exit status if files don't exist.
 *                  Overrides any previous -i arguments.
 *
 *     -i           Prompt for confirmation before removing any files.
 *
 *     -r           Remove file hierarchies (aka "recursive")
 *     -R           Equivalent to -r.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */


#include <ftw.h>
#include <err.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include "boreutils.h"

// https://pubs.opengroup.org/onlinepubs/9699919799/utilities/rm.html

static char *name = NULL;

static int dash_f = 0;
static int dash_i = 0;
static int dash_r = 0;

static int prompt(const char *fpath, char *descriptor) {
    if (!dash_i) {
        return 0;
    }

    fprintf(stderr, "%s: remove %s %s? ", name, descriptor, fpath);

    char buf[3] = {0};
    read(STDIN_FILENO, buf, 2);

    if (buf[0] == 'y' || buf[0] == 'Y') {
        return 0;
    }

    return -1;
}

static int rmtree_cb(const char *fpath, const struct stat *sb, int tflag,
        struct FTW *ftwbuf) {
    (void)sb;
    (void)ftwbuf;

    if (tflag == FTW_NS || tflag == FTW_DNR) { // permission error
        if (!dash_f) {
            err(1, "cannot remove '%s'", fpath);
        }
    } else if (tflag == FTW_DP) { // directory (after children were visited)
        if (dash_i && prompt(fpath, "directory") == -1) {
            return 0;
        }
        rmdir(fpath);
    } else if (tflag == FTW_F) { // file
        if (dash_i && prompt(fpath, "normal file") == -1) {
            return 0;
        }
        unlink(fpath);
    } else if (tflag == FTW_SL || tflag == FTW_SLN) { // symlink
        if (dash_i && prompt(fpath, "symlink") == -1) {
            return 0;
        }
        unlink(fpath);
    }

    return 0; // tell nftw to continue.
}

static int rmtree(char *fpath) {
   struct stat statbuf = {0};
   if (stat(fpath, &statbuf) != 0) {
       return 0;
    }

   if (S_ISDIR(statbuf.st_mode)) {
        // FTW_DEPTH = report files first, then their parent directories.
        // FTW_PHYS  = don't follow symlinks; report the symlinks themselves.
        return nftw(fpath, rmtree_cb, 20 /* fd_limit */, FTW_DEPTH | FTW_PHYS);
    } else {
        // nftw() on at least macOS does not like fpath being a file.
        // So we handle that case by directly calling rmtree_cb().
        return rmtree_cb(fpath, NULL, FTW_F, NULL);
    }
}


int main(int argc, char **argv)
{
    name = argv[0];
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: rm [-rRi] file...");
        puts("       rm -f [-rRi] [file]...");
        puts("Delete specified file(s).");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    int i = 1;
    for (; i < argc; i++) {
        if (argv[i][0] != '-') {
            break;
        }

        for (size_t j = 1; j < strlen(argv[i]); j++) {
            char cur = argv[i][j];
            if (cur == 'r' || cur == 'R') {
                dash_r = 1;
            } else if (cur == 'i') {
                dash_f = 0;
                dash_i = 1;
            } else if (cur == 'f') {
                dash_f = 1;
                dash_i = 0;
            } else {
                fputs("rm: unknown argument: -", stderr);
                fputc(cur, stderr);
                fputs("\n", stderr);
                return 1;
            }
        }
    }

    // No files specified.
    if (i == argc) {
        if (dash_f) {
            return 0;
        }
        bu_missing_argument(argv[0]);
        return 1;
    }

    for (; i < argc; i++) {
        // If the file does not exist...
        if (access(argv[i], F_OK) == -1) {
            if (!dash_f) {
                err(1, "cannot remove '%s'", argv[i]);
            }
            continue;
        }


        if (dash_r) {
            if (rmtree(argv[i]) == -1 && !dash_f) {
                err(1, "cannot remove '%s'", argv[i]);
            }
        } else {
            if (dash_i && prompt(argv[i], "file") == -1) {
                continue;
            }
            if (unlink(argv[i]) == -1 && !dash_f) {
                err(1, "cannot remove '%s'", argv[i]);
            }
        }
    }

    return 0;
}
