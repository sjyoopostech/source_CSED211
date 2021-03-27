#include <stdio.h>
#include <inttypes.h>
uint64_t time_stamp()
{
    uint64_t rdx, rax;
    __asm("rdtsc" : "=a" (rax), "=d"(rdx));
        return rax | (rdx << 32);
}
int main()
{
    int i, j;
    uint64_t start, end;
    char c[2000][2000];
    char a = '2';
    start = time_stamp();
    for (i = 0; i < 2000; i++) {
        for (j = 0; j < 2000; j++) {
            c[i][j] = a;
        }
    }
    end = time_stamp();
    printf("1. row-order execution time : %"PRIu64"\n", end-start);
    char c_[2000][2000];
    char a_ = '3';
    start = time_stamp();
    for (i = 0; i < 2000; i++) {
        for (j = 0; j < 2000; j++) {
            c_[j][i] = a_;
        }
    }
    end = time_stamp();
    printf("2. column-order execution time : %"PRIu64"\n", end-start);
    return 0;
}
