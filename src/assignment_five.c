/*
write a program search for file in folder
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

int file_found = 0;
int search_file(const char *directory, const char *filename) {
  DIR *dir = opendir(directory);

  if (dir == NULL) {
    printf("Error opening directory %s\n", directory);
    return -1;
  }

  struct dirent *entry = NULL;

  while (!file_found && (entry = readdir(dir)) != NULL) {

    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    struct stat file_stat;
    char full_path[1024];
    snprintf(full_path, sizeof(full_path), "%s/%s", directory, entry->d_name);

    if (stat(full_path, &file_stat) == -1) {
      printf("Error getting file status for %s\n", entry->d_name);
      continue;
    }

    if (S_ISDIR(file_stat.st_mode)) {
      char subdir[1024];
      snprintf(subdir, sizeof(subdir), "%s/%s", directory, entry->d_name);
      search_file(subdir, filename);
      continue;
    }

    printf("Checking file: %s\n", entry->d_name);
    if (!file_found && strcmp(entry->d_name, filename) == 0) {
      printf("File %s found in directory %s\n", filename, directory);
      file_found = 1;
      break;
    }
  }

  if (entry == NULL) {
    printf("File %s not found in directory %s\n", filename, directory);
  }
  closedir(dir);
  return file_found ? 0 : -1;
}


int main(void) {
  const char *directory;
  #ifdef _WIN32
    directory = "E:\\arjun\\Programming\\C ++\\Practice";
  #elif __linux__
    directory = "/home/arjun/C++_Program";
  #endif
  const char *filename = "part_001.txt";
  int result = search_file(directory, filename);
  if (result == -1) {
    printf("Error occur to search for file %s in directory %s\n", filename, directory);
    return -1;
  }
  return 0;
}