#include <iostream>
#include <dirent.h> 
#include <sys/stat.h>
#include <string.h>

#ifdef _WIN32
  #define PATH_SEPARATOR '\\'
#else
  #define PATH_SEPARATOR '/'
#endif

bool file_found = false;

bool search_file(const std::string &directory, const std::string &filename)
{
  DIR *dir = opendir(directory.c_str());
  if (dir == nullptr)
  {
    printf("Error opening directory %s\n", directory.c_str());
    return false;
  }

  struct dirent *entry = nullptr;

  while (!file_found && (entry = readdir(dir)) != nullptr)
  {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
    {
      continue;
    }

    struct stat file_stat;

    std::string full_path = directory + PATH_SEPARATOR + entry->d_name;

    if (stat(full_path.c_str(), &file_stat) == -1)
    {
      printf("Error getting file status for %s\n", entry->d_name);
      continue;
    }

    if (S_ISDIR(file_stat.st_mode))
    {
      std::string subdir = directory + PATH_SEPARATOR + entry->d_name;
      search_file(subdir, filename);
      continue;
    }

    if (!file_found && strcmp(entry->d_name, filename.c_str()) == 0) 
    {
      file_found = true;
      break;
    }
  }

  return file_found;
}

int main(int argc, char *args[]) 
{
  if (argc != 3)
  {
    printf("arguments: file name and folder name missing\n");
    return EXIT_FAILURE;
  }

  const std::string file_name = args[1];
  const std::string folder_name = args[2];

  int result = search_file(folder_name, file_name);

  if (result)
  {
    printf("File %s found in directory %s\n", file_name.c_str(), folder_name.c_str());
    return EXIT_SUCCESS;
  }
  else
  {
    printf("File %s not found in directory %s\n", file_name.c_str(), folder_name.c_str());
    return EXIT_FAILURE;
  }
}