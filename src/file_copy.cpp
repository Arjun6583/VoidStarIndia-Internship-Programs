#include <file_copy.h>
#include <iostream>
#include <fcntl.h>

bool copy_file_using_fopen(const std::string &source_path, const std::string &destination_path) 
{
  printf("Copying file from %s to %s\n", source_path.c_str(), destination_path.c_str());

  FILE *source_file_pointer = fopen(source_path.c_str(), "rb");
  if (source_file_pointer == nullptr) 
  {
    printf("Error opening source file: %s\n", source_path.c_str());
    return false;
  }

  FILE *destination_file_pointer = fopen(destination_path.c_str(), "wb");
  if (destination_file_pointer == nullptr) 
  {
    printf("Error opening destination file: %s\n", destination_path.c_str());
    fclose(source_file_pointer);
    return false;
  }

  size_t bytes_read = 0;
  char *pch_buffer = new char[1024];

  while ((bytes_read = fread(pch_buffer, 1, sizeof(pch_buffer), source_file_pointer)) > 0)
  {
    size_t bytes_written = fwrite(pch_buffer, 1, bytes_read, destination_file_pointer);
    if (bytes_written < bytes_read) 
    {
      printf("Error writing to destination file: %s\n", destination_path.c_str());
      fclose(source_file_pointer);
      fclose(destination_file_pointer);
      delete[] pch_buffer;
      return false;
    }
  }
  fclose(source_file_pointer);
  fclose(destination_file_pointer);
  delete[] pch_buffer;
  return true;
}

bool copy_file_using_open(const std::string &source_path, const std::string &destination_path) 
{
  printf("Copying file from %s to %s\n", source_path.c_str(), destination_path.c_str());
  #ifndef __linux__
    printf("Function not supported on Windows systems.\n");
    return false;
  #endif

  int i_source_fd = open(source_path.c_str(), O_RDONLY);
  if (i_source_fd < 0) 
  {
    perror("Error opening source file");
    return false;
  }

  int destination_fd = open(destination_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
  if (destination_fd < 0) 
  {
    perror("Error opening destination file");
    close(i_source_fd);
    return false;
  }

  char *pch_buffer = new char[1024];
  ssize_t bytes_read;

  while ((bytes_read = read(i_source_fd, pch_buffer, sizeof(pch_buffer))) > 0) 
  {
    ssize_t bytes_written = write(destination_fd, pch_buffer, bytes_read);
    if (bytes_written < bytes_read) 
    {
      perror("Error writing to destination file");
      close(i_source_fd);
      close(destination_fd);
      return false;
    }
  }

  close(i_source_fd);
  close(destination_fd);
  return true;
}

bool copy_file_using_fopen_s(const std::string &source_path, const std::string &destination_path) 
{
  printf("Copying file from %s to %s\n", source_path.c_str(), destination_path.c_str());
  #ifndef _WIN32
    printf("Function not supported on Windows systems.\n");
    return false;
  #endif
  FILE *source_file_pointer = nullptr;
  errno_t res = fopen_s(&source_file_pointer, source_path.c_str(), "rb");
  if (res != 0 || source_file_pointer == nullptr) 
  {
    printf("Error opening source file: %s\n", source_path.c_str());
    return false;
  }

  FILE *destination_path_pointer = nullptr;
  res = fopen_s(&destination_path_pointer, destination_path.c_str(), "wb");
  if (res != 0 || destination_path_pointer == nullptr) 
  {
    printf("Error opening destination file: %s\n", destination_path.c_str());
    fclose(source_file_pointer);
    return false;
  }

  size_t bytes_read = 0;
  char *pch_buffer = new char[1024];
  while ((bytes_read = fread_s(pch_buffer, sizeof(pch_buffer), 1, sizeof(pch_buffer), source_file_pointer)) > 0) 
  {
    size_t bytes_written = fwrite(pch_buffer, 1, bytes_read, destination_path_pointer);
    if (bytes_written < bytes_read) 
    {
      printf("Error writing to destination file: %s\n", destination_path.c_str());
      fclose(source_file_pointer);
      fclose(destination_path_pointer);
      delete[] pch_buffer;
      return false;
    }
  }

  fclose(source_file_pointer);
  fclose(destination_path_pointer);
  delete[] pch_buffer;
  return true;
}

bool check_platform_file_path(const std::string &source_path, const std::string &destination_path) 
{
  #ifdef _WIN32
    for(const char &c : source_path) 
    {
      if (c == '/') 
      {
        printf("Invalid path: Windows does not support forward slashes.\n");
        return false;
      }
    }
    for(const char &c : destination_path) 
    {
      if (c == '/') 
      {
        printf("Invalid path: Windows does not support forward slashes.\n");
        return false;
      }
    }
  #elif __linux__
    for(const char &c : source_path) 
    {
      if (c == '\\') 
      {
        printf("Invalid path: Linux does not support backslashes.\n");
        return false;
      }
    }
    for(const char &c : destination_path) 
    {
      if (c == '\\') 
      {
        printf("Invalid path: Linux does not support backslashes.\n");
        return false;
      }
    }
  #endif
  return true;
}