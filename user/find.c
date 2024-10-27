#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define PATH_SIZE 128

int
find(const char*, const char*);

const char*
cstr_rfind_util(const char*, char);

int
main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s [dir] [filename]", argv[0]);
        exit(1);
    }

    exit(find(argv[1], argv[2]));
}

int
find(const char* path, const char* target) {
    struct stat st;
    if (stat(path, &st) < 0) {
        fprintf(2, "cannot stat %s", path);
        return 1;
    }

    if (strcmp(cstr_rfind_util(path, '/'), target) == 0) {
        printf("%s\n", path);
    }

    if (st.type != T_DIR) {
        return 0;
    }

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        fprintf(2, "path %s not exists, %d\n", path, strlen(path));
        return 1;
    }

    char next_path[256];
    strcpy(next_path, path);
    char* next_path_ptr = next_path + strlen(next_path);
    *next_path_ptr++ = '/';

    struct dirent de;
    while (read(fd, &de, sizeof de) == sizeof(de)) {
        if (de.inum == 0) {
            continue;
        }
        if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) {
            continue;
        }

        strcpy(next_path_ptr, de.name);
        find(next_path, target);
    }

    return 0;
}

const char*
cstr_rfind_util(const char* str, char c) {
    int offset = 0;
    const char* ptr = str;
    while (*ptr != 0) {
        if (*ptr == '/') {
            offset = ptr - str + 1;
        }
        ++ptr;
    }
    return str + offset;
}

