
#include <iostream>
#include "file_copy.h"

int main(int argc, char *args[]) 
{
  if (argc != 3) 
  {
    printf("arguments: source file destination file path missing\n");
    return EXIT_FAILURE;
  }

  const std::string source_file_path = args[1];
  const std::string destination_file_path = args[2];
  bool valid_path = check_platform_file_path(source_file_path, destination_file_path);
  if (!valid_path) 
  {
    printf("Invalid file paths provided.\n");
    return EXIT_FAILURE;
  }

  bool result = copy_file_using_fopen_s(source_file_path, destination_file_path);
  if (result) 
  {
    printf("File copied successfully from %s to %s\n", source_file_path.c_str(), destination_file_path.c_str());
    return EXIT_SUCCESS;
  }

  printf("File copy failed from %s to %s\n", source_file_path.c_str(), destination_file_path.c_str());
  return EXIT_FAILURE;
}