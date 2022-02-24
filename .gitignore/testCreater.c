#include <stdio.h>
#include <limits.h>
#include <stdint.h>

int Fabs(int x)
{
    return (x >= 0) ? x : -x;
}

int main(void)
{
    freopen("out.txt", "w", stdout);

    int index = 1;
    int sum = 1;

    while (sum <= INT16_MAX)
    {
        printf("%d %d\n", index, sum);
        printf("%d %d\n", -sum, -index);
        ++index;
        sum += index;
    }    
    
    while (sum >= 0)
    {
        printf("%d %d\n", index, sum);
        printf("%d %d\n", -sum, -index);
        ++index;
        sum += index;
    }    
    return 0;
}