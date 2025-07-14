#include <stdio.h>
#include "file_copy.h"

int main(int argc, char *args[])
{
  if (argc != 3)
  {
    printf("arguments: source file path destination file path missing\n");
    return EXIT_FAILURE;
  }

  const char *source_path = args[1];
  const char *destination_path = args[2];

  bool result = vsi_create_file(source_path, destination_path);
  if (result == false)
  {
    printf("Failed to create file from %s to %s\n", source_path, destination_path);
    return EXIT_FAILURE;
  }

  printf("File created successfully from %s to %s\n", source_path, destination_path);
  return EXIT_SUCCESS;
}