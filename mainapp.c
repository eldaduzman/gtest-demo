#include <stdio.h>
#include "modules/calculations.h"

int main(int argc, char *argv[])
{
    int arr[] = {1,5,4,6,7,9,8,10,2};
    size_t n = sizeof(arr)/sizeof(arr[0]);
    float mean = calculate_mean(arr, n);
    printf("Mean=%.2f\n", mean);
    return 0;
}
