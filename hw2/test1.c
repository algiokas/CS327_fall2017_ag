#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, int *argv[])
{
    unsigned char a = 'A';
    unsigned char b = 'B';
    unsigned char c = 'C';
    unsigned char d = '4';

    unsigned char *test1 = (unsigned char *) malloc(5 * sizeof(unsigned char));
    unsigned char test2[4];

    test1[0] = a;
    test2[0] = a;

    test1[1] = b;
    test2[1] = b;

    test1[2] = c;
    test2[2] = c;
    
    test1[3] = d;
    test2[3] = d;
    
    test1[4] = '\0';

    printf("String 1 : %s \n", test1);
    printf("String 2 : %s \n", test2);

    printf("String Compare 1 : %d \n", strcmp(test1, "ABC3"));
    printf("String Compare 2 : %d \n", strcmp(test2, "ABC3"));

    int32_t int1;
    int32_t int2;

    memcpy(&int1, test1, 4);
    memcpy(&int2, test2, 4);

    printf("Int 1 : %d \n", int1);
    printf("Int 2 : %d \n", int2);

    return 0;
}
