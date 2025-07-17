/*
write a program create 10MB file with random character and number and save it
*/

#include <stdio.h>
#include <stdlib.h>
#define FILE_SIZE (10 * 1024 * 1024)
int make_file(void) {
  FILE *fp;
  fp = fopen("random_file.txt", "w");

  if(fp == NULL) {
    printf("Error opening file!\n");
    return -1;
  }
  int cnt = 1;
  
  const char character_set[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz?!@#$%^&*()-_=+[]{}|;:',.<>/?`~";
  int len = sizeof(character_set) - 1;
  while(ftell(fp) < FILE_SIZE) {
    char ch = character_set[rand() % len];

    if (fputc(ch, fp) == EOF) {
      printf("Error writing to file!\n");
      fclose(fp);
      return -1;
    }
  }

  fclose(fp);
  return 0;
}

int main(void) {
  int result = make_file();
  if(result != 0) {
    printf("Failed to create file.\n");
    return -1;
  }
  printf("File created successfully.\n");
  return 0;
}