#include "kernel/types.h"
#include "user/user.h"

void debug_f(){
    int *ptr = malloc(16);
    *ptr = 10;
    printf("pointer : %x\n", *ptr);
}

int main() {
    debug_f();
    return 0;
}

