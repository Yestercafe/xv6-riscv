#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char* argv[]) {
    char buf[5];
    int p[2];

    pipe(p);

    int pid = fork();
    if (pid > 0) {  // parent
        strcpy(buf, "ping");
        write(p[1], buf, sizeof buf);
        wait(0);
        read(p[0], buf, sizeof buf);
        printf("%d: received %s\n", getpid(), buf);
    } else if (pid == 0) {  // child
        read(p[0], buf, sizeof buf);
        printf("%d: received %s\n", getpid(), buf);
        strcpy(buf, "pong");
        write(p[1], buf, sizeof buf);
    } else {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    exit(0);
}

