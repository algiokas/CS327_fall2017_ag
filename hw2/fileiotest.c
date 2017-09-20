#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
    char *home = getenv("HOME");
    printf("HOME : %s\n", home);

    char *rpath = "/test/test2.txt";
    printf("RELATIVE PATH : %s\n", rpath);
    
    char *path = (char *) malloc((strlen(home) + strlen(rpath) + 2) * sizeof(char));
    strcpy(path, home);
    strcat(path, rpath);

    printf("PATH : %s\n", path);

    FILE *f;
    f = fopen(path, "w");
    fputs("RLG327", f);

    int32_t version = 0;
    int32_t size = 1718;

    printf("Version : %d ,sizeof(version) : %d\n", version, sizeof(version));
    printf("size : %d ,sizeof(size) : %d\n", size, sizeof(size));

    unsigned char *le = (unsigned char*) &size;
    unsigned char *buff = (unsigned char*) malloc(sizeof(size));

    int j;
    for (j = 0; j < 4; j++) {
        buff[j] = le[3-j];
    }

    fwrite(&version, sizeof(version), 1, f);
    fwrite(buff, sizeof(size), 1, f);

    int i;
    for (i = 0; i < 20; i++) {
        fputc(1, f);
    }

    fclose(f);

    return 0;
}
    
