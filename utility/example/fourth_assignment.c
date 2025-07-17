/*
write a program to copy a file from one location to another given folder
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int copy_file(const char *source, const char *destination_folder) {
  FILE *src_fp = fopen(source, "rb");
  if (src_fp == NULL) {
    printf("Error opening source file: %s\n", source);
    return -1;
  }

  FILE *dest_fp;
  char buffer[1024];
  size_t bytes_read;
  char *destination_path = calloc(strlen(destination_folder) + strlen(source) + 2, sizeof(char));
  if (destination_path == NULL) {
    printf("Memory allocation failed\n");
    fclose(src_fp);
    return -1;
  }
  strcat(destination_path, destination_folder);
  strcat(destination_path, "\\");
  strcat(destination_path, source);
  dest_fp = fopen(destination_path, "wb");
  if (dest_fp == NULL) {
    printf("Error opening destination file: %s\n", destination_path);
    free(destination_path);
    fclose(src_fp);
    return -1;
  }

  while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_fp)) > 0) {
    if (fwrite(buffer, 1, bytes_read, dest_fp) != bytes_read) {
      printf("Error writing to destination file: %s\n", destination_path);
      fclose(src_fp);
      fclose(dest_fp);
      free(destination_path);
      return -1;
    }
  }
  fclose(src_fp);
  fclose(dest_fp);
  free(destination_path);
  destination_path = NULL;
  return 0;
}

int main(void) {
  const char *destination_folder;
  #ifdef _WIN32
      destination_folder = "E:\\arjun\\Programming\\C ++\\Practice"; 
  #elif __linux__
      destination_folder = "/home/arjun/C++_Program/"; 
  #endif
  const char *source_file = "source.txt";
  int result = copy_file(source_file, destination_folder);
  if (result == 0) {
    printf("File copied successfully from %s to %s\n", source_file, destination_folder);
  } else {
    printf("Failed to copy file from %s to %s\n", source_file, destination_folder);
    return -1;
  }
  return 0;
}