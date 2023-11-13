#include <omp.h>
#include <stdio.h>

auto main() -> int {
    #pragma omp parallel 
    printf("hello world\n");
}
