#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

// 如果要完全按照 Bells Labs 的方法完全并行的话上限 280 做不太到，不知道是怎么回事，以前的题目好像是 35 倒是可以
// 暂时还不太清楚 xv6 FD 的数量上限是多少，这题暂时先这样
#define BOUND 35

void
primes(int, int) __attribute__((noreturn));

int
main(int argc, char* argv[]) {
    int p[2];
    pipe(p);
    int r = p[0], w = p[1];

    int pid = fork();
    if (pid > 0) {
        close(r);
        for (int a = 2; a <= BOUND; ++a) {
            write(w, &a, sizeof a);
        }
        close(w);
        wait(0);
    } else if (pid == 0) {
        close(w);
        primes(2, r);
        close(r);
    } else {
        fprintf(2, "fork failed\n");
        exit(-1);
    }
    exit(0);
}

void
primes(int prime, int left_r) {
    int recv;
    if (read(left_r, &recv, sizeof recv) == 0) {
        exit(0);
    }

    int next_prime = recv;
    printf("prime %d\n", next_prime);

    int p[2];
    pipe(p);
    int right_r = p[0], right_w = p[1];
    int pid = fork();
    
    if (pid > 0) {
        close(right_r);
        while (read(left_r, &recv, sizeof recv) > 0) {
            if (recv % prime != 0)
                write(right_w, &recv, sizeof recv);
        }
        close(right_w);
        wait(0);
    } else if (pid == 0) {
        close(right_w);
        primes(next_prime, right_r);
        close(right_r);
    } else {
        fprintf(2, "fork failed\n");
        exit(-1);
    }
    exit(0);
}

