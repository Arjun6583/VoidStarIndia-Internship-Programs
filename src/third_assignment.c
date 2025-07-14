/*
write a program copy a file from one location to another
*/

#include <stdio.h>
#include <stdlib.h>

int copy_file(const char *source, const char *destination) {
    FILE *src_fp = fopen(source, "rb");
    if (src_fp == NULL) {
        perror("Error opening source file");
        return -1;
    }

    FILE *dest_fp = fopen(destination, "wb");
    if (dest_fp == NULL) {
        perror("Error opening destination file");
        fclose(src_fp);
        return -1;
    }

    char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_fp)) > 0) {
        if (fwrite(buffer, 1, bytes_read, dest_fp) != bytes_read) {
            printf("Error writing to destination file");
            fclose(src_fp);
            fclose(dest_fp);
            return -1;
        }
    }

    fclose(src_fp);
    fclose(dest_fp);
    return 0;
}

int main(void) {
    const char *destination_file;
    #ifdef _WIN32
        destination_file = "E:\\arjun\\Programming\\C ++\\Practice\\destination.txt"; 
    #elif __linux__
        destination_file = "/home/arjun/C++_Program/destination.txt"; 
    #endif
    const char *sourc_file = "source.txt";
    int result = copy_file(sourc_file, destination_file);

    if(result == 0) {
      printf("File copied successfully from %s to %s\n", sourc_file, destination_file);
    } else {
      printf("Failed to copy file from %s to %s\n", sourc_file, destination_file);
      return -1;
    }

    return 0;
}