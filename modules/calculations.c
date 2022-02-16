#include <stdio.h>
#include "calculations.h"
float calculate_mean(int arr[], size_t size)
{
    if(size == 0)
    {
        return 0;
    }
    int sum = 0;
    int i = 0;
    for (i = 0; i < size; i++)
    {
        sum += arr[i];
    }
    return (float)sum / size;
}