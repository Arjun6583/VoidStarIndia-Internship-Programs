/*
write program split 100MB file into 10MB each
*/
#include <stdio.h>
#include <stdlib.h>

#define CHUNK (10 * 1024 * 1024)     

int split_file(const char *source_file)
{
    FILE *src = fopen(source_file, "r");
    if (!src) {
        printf("Error opening source file: %s\n", source_file);
        return -1;
    }

    char *buffer = (char *)malloc(CHUNK  * sizeof(char));
    if (!buffer) {
        printf("Fail to create chuck !!!");
        fclose(src);
        return -1;
    }

    int part_no = 0;
    size_t bytes_in_part = 0;
    FILE *part = NULL;

    while (1) {
        if (!part) {
            char name[64];
            snprintf(name, sizeof(name), "part_%03d.txt", part_no++);
            part = fopen(name, "w");
            if (!part) {
                printf("\nFile not created!!!");
                free(buffer);
                fclose(src);
                return -1;
            }
            bytes_in_part = 0;
        }

        size_t to_read = CHUNK - bytes_in_part;
        size_t n = fread(buffer, 1, to_read, src);

        if (n == 0) { 
            break;
        }

        if (fwrite(buffer, 1, n, part) != n) {
            printf("Error writing file");
            free(buffer);
            fclose(src);
            fclose(part);
            return -1;
        }

        bytes_in_part += n;

        if (bytes_in_part == CHUNK) {
            fclose(part);
            part = NULL;
        }
    }

    if (part) fclose(part);
    free(buffer);
    fclose(src);
    buffer = NULL;
    return 0;
}

int main(void)
{
    const char *file = "random_file.txt";
    if (split_file(file) == 0)
        printf("File split successfully.");
    else
        printf("Splitting failed.");
    return 0;
}
