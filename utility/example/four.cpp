#include<iostream>
#include "file_copy.h"

#ifdef _WIN32
    #include <direct.h>
    #define ACCESS _access
    #define MKDIR(dir) _mkdir(dir)
#else
    #include <unistd.h>
    #include <sys/stat.h>
    #define ACCESS access
    #define MKDIR(dir) mkdir(dir, 0755)
#endif


bool copy_file_to_given_folder(const std::string &source_file_path, const std::string &destination_folder)
{
  bool valid_paths = check_platform_file_path(source_file_path, destination_folder);
  if (!valid_paths)
  {
    printf("Invalid file paths provided.\n");
    return false;
  }

  if (ACCESS(destination_folder.c_str(), 0) != 0)
  {
    int result = MKDIR(destination_folder.c_str());
    if (result != 0)
    {
      printf("Failed to create destination folder: %s\n", destination_folder.c_str());
      return false;
    }
  }

  std::string destination_path = "";

  #ifdef _WIN32
    destination_path = destination_folder + "\\" + source_file_path.substr(source_file_path.find_last_of("\\/") + 1);
  #else
    destination_path = destination_folder + "/" + source_path.substr(source_path.find_last_of("\\/") + 1);
  #endif

  bool result = copy_file_using_fopen(source_file_path, destination_path);
  if (result)
  {
    printf("File copied successfully from %s to %s\n", source_file_path.c_str(), destination_path.c_str());
    return true;
  }

  printf("File copy failed from %s to %s\n", source_file_path.c_str(), destination_path.c_str());
  return false;
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("arguments: source_file destination_folder missing\n");
    return EXIT_FAILURE;
  }

  const std::string source_file_path = argv[1];
  const std::string destination_folder = argv[2];

  bool result = copy_file_to_given_folder(source_file_path, destination_folder);

  if (result == false)
  {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}