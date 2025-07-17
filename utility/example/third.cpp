#include "file_copy.h"
#include <iostream>

int main(int argc, char *argv[]) 
{
  if(argc != 3)
  {
    printf("arguments: source_file destination_file missing\n");
    return EXIT_FAILURE;
  }
  
  const std::string source_path = argv[1];
  const std::string destination_path = argv[2];
  bool valid_paths = check_platform_file_path(source_path, destination_path);
  if (!valid_paths) 
  {
    printf("Invalid file paths provided.\n");
    return EXIT_FAILURE;
  }

  bool result = copy_file_using_fopen(source_path, destination_path);
  if (result == true) 
  {
    printf("File copied successfully from %s to %s\n", source_path.c_str(), destination_path.c_str());
    return EXIT_SUCCESS;
  } 
  printf("File copy failed from %s to %s\n", source_path.c_str(), destination_path.c_str());
  return EXIT_FAILURE;
}