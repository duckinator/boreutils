/**
 * NAME
 * ====
 *     id - print user identity information
 *
 * SYNOPSIS
 * ========
 *     id [user]
 *     id -G [-n] [user]
 *     id -g [-nr] [user]
 *     id -u [-nr] [user]
 *     id [--help|--version]
 *
 * DESCRIPTION
 * ===========
 *     Print user identity information.
 *
 *     -G           Print only all different group IDs.
 *     -g           Print only the effective group ID.
 *     -u           Print only the effective user ID.
 *     -n           Print names, not numeric IDs.
 *     -r           Print the real ID instead of the effective ID.
 *
 *     --help       Print help text and exit.
 *     --version    Print version information and exit.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include "boreutils.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
typedef struct Options_s {
    char *user;
    int dash_G;
    int dash_g;
    int dash_u;
    int dash_n;
    int dash_r;
} Options;

typedef struct Ids_s {
    char *login;
    uid_t uid;
    uid_t euid;
    gid_t gid;
    gid_t egid;
} Ids;

typedef struct GroupList_s {
    gid_t *groups;
    int size;
} GroupList;
#pragma clang diagnostic pop

// getgrouplist() is nonstandard, so I wound up reimplementing it myself
// using only the standard getgrent() and endgrent().
static int _bu_getgrouplist(const char *name, gid_t basegid, gid_t *groups,
        int *ngroups) {
    struct group *grp = NULL;
    int max_groups = *ngroups;
    int num_groups = 0;

    if (max_groups > 0) {
        groups[0] = basegid;
    }
    num_groups++;

    while ((grp = getgrent())) {
        char *mem_name = NULL;
        for (size_t i = 0; grp->gr_mem[i] != NULL; i++) {
            mem_name = grp->gr_mem[i];
            if (strcmp(mem_name, name) == 0) {
                if (num_groups < max_groups) {
                    groups[num_groups] = grp->gr_gid;
                }
                num_groups++;
                break;
            }
        }
    }

    endgrent();

    *ngroups = num_groups;

    if (num_groups > max_groups) {
        return -1;
    } else {
        return 0;
    }
}

static void get_group_list(GroupList *group_list, uid_t uid) {
    struct passwd* pw = getpwuid(uid);
    if(pw == NULL){
        perror("getpwuid error: ");
    }

    // Get number of groups.
    _bu_getgrouplist(pw->pw_name, pw->pw_gid, NULL, &(group_list->size));

    // Allocate group list info.
    size_t size = sizeof(gid_t) * (size_t)group_list->size;
    group_list->groups = malloc(size);
    memset(group_list->groups, 0, size);

    // Get groups.
    _bu_getgrouplist(pw->pw_name, pw->pw_gid, group_list->groups, &(group_list->size));
}

static int parse_args(int argc, char **argv, Options *options) {
    for (int i = 1; i < argc; i++) {
        if (options->user != NULL) {
            bu_extra_argument(argv[0]);
            return -1;
        }

        if (argv[i][0] == '-') {
            for (size_t j = 1; j < strlen(argv[i]); j++) {
                if (argv[i][j] == 'G') { options->dash_G = 1; }
                else if (argv[i][j] == 'g') { options->dash_g = 1; }
                else if (argv[i][j] == 'u') { options->dash_u = 1; }
                else if (argv[i][j] == 'n') { options->dash_n = 1; }
                else if (argv[i][j] == 'r') { options->dash_r = 1; }
                else {
                    char arg[3] = {'-', argv[i][j], 0};
                    bu_invalid_argument(argv[0], arg);
                }
            }
        } else {
            options->user = argv[i];
        }
    }
    return 0;
}

static void print_user(char *prefix, uid_t uid) {
    struct passwd *passwd_obj = getpwuid(uid);
    char *user_name = passwd_obj->pw_name;

    printf("%s%u", prefix, uid);
    if (user_name) { printf("(%s)", user_name); }
}

static void print_group_name(gid_t gid) {
    struct group *group_obj = getgrgid(gid);

    fputs(group_obj->gr_name, stdout);
}

static void print_group(char *prefix, gid_t gid) {
    struct group *group_obj = getgrgid(gid);
    char *group_name = group_obj->gr_name;

    printf("%s%u", prefix, gid);
    if (group_name) { printf("(%s)", group_name); }
}

static int main_default(Options *options, Ids *ids) {
    (void)options; // unused.

    print_user("uid=", ids->uid);

    print_group(" gid=", ids->gid);

    if (ids->uid != ids->euid) {
        print_user(" euid=", ids->euid);
    }

    if (ids->gid != ids->egid) {
        print_group(" egid=", ids->egid);
    }

    GroupList group_list = {0};
    get_group_list(&group_list, ids->uid);

    print_group(" groups=", group_list.groups[0]);
    for (int i = 1; i < group_list.size; i++) {
        print_group(",", group_list.groups[i]);
    }

    free(group_list.groups);

    puts("");

    return 0;
}

static int main_user(Options *options, Ids *ids) {
    if (options->dash_n) {
        printf("%s\n", ids->login);
    } else {
        printf("%u\n", ids->uid);
    }
    return 0;
}

static int main_list_groups(Options *options, Ids *ids) {
    GroupList group_list = {0};
    get_group_list(&group_list, ids->uid);

    for (int i = 0; i < group_list.size; i++) {
        if (i > 0) { fputs(" ", stdout); }

        if (options->dash_n) {
            print_group_name(group_list.groups[i]);
        } else {
            printf("%u", group_list.groups[i]);
        }
    }

    free(group_list.groups);

    puts("");

    return 0;
}

static int main_group(Options *options, Ids *ids) {
    if (options->dash_n) {
        struct group *group_obj = getgrgid(ids->gid);
        printf("%s\n", group_obj->gr_name);
    } else if (options->dash_r) {
        printf("%u\n", ids->gid);
    } else {
        printf("%u\n", ids->egid);
    }

    return 0;
}

static int populate_ids(Options *options, Ids *ids) {
    if (options->user) {
        struct passwd *pw = getpwnam(options->user);

        if (pw == NULL) {
            fprintf(stderr, "id: no such user: %s\n", options->user);
            return -1;
        }

        ids->login = pw->pw_name;
        ids->uid = pw->pw_uid;
        ids->gid = pw->pw_gid;

        // Effective IDs aren't useful in this context, so
        // we the effective IDs to the actual IDs.
        ids->euid = ids->uid;
        ids->egid = ids->gid;
    } else {
        ids->login = getlogin();
        ids->uid = getuid();
        ids->gid = getgid();
        ids->euid = geteuid();
        ids->egid = getegid();
    }


    return 0;
}

int main(int argc, char **argv) {
    if (has_arg(argc, argv, "-h") || has_arg(argc, argv, "--help")) {
        puts("Usage: id [user]");
        puts("       id -G [-n] [user]");
        puts("       id -g [-nr] [user]");
        puts("       id -u [-nr] [user]");
        puts("");
        puts("Print user identity information.");
        puts("");
        puts("-G  Print only all unique group IDs");
        puts("-g  Print only the effective group ID");
        puts("-u  Print only the effective user ID");
        puts("-n  Show names, not numeric IDs");
        puts("-r  Show the real ID instead of the effective ID");
        return 1;
    }

    if (bu_handle_version(argc, argv)) {
        return 0;
    }

    Options options = {0};
    Ids ids = {0};

    if (parse_args(argc, argv, &options) == -1) {
        return 1;
    }

    if ((options.dash_G + options.dash_g + options.dash_u) > 1) {
        fprintf(stderr, "%s: only one of -G, -g, and -u can be used\n", argv[0]);
        return 1;
    }

    if (((options.dash_G + options.dash_g + options.dash_u) == 0) &&
        ((options.dash_n + options.dash_r) > 0)) {
        fprintf(stderr, "%s: -n and -r require one of -G, -g, or -u\n", argv[0]);
        return 1;
    }

    if (populate_ids(&options, &ids) == -1) {
        return 1;
    }

    if (options.dash_u) {
        return main_user(&options, &ids);
    } else if (options.dash_g) {
        return main_group(&options, &ids);
    } else if (options.dash_G) {
        return main_list_groups(&options, &ids);
    } else {
        return main_default(&options, &ids);
    }
}
