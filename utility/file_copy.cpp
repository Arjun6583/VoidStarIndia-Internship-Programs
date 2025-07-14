#include <cstdio>
#include <string>
#include "file_copy.h"

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/stat.h>
#endif 

#define BUFFER_SIZE 1024


bool vsi_create_file(const std::string& source_path, const std::string& destination_path) 
{
  #ifdef _WIN32
    HANDLE h_source = CreateFileA(
      source_path.c_str(),
      GENERIC_READ,
      FILE_SHARE_READ,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL
    );
    if (h_source == INVALID_HANDLE_VALUE)
    {
      printf("Failed to open source file: %s\n", source_path.c_str());
      return false;
    }

    HANDLE h_destination = CreateFileA(
      destination_path.c_str(),
      GENERIC_WRITE,
      0,
      NULL,
      CREATE_ALWAYS,
      FILE_ATTRIBUTE_NORMAL,
      NULL
    );
    if (h_destination == INVALID_HANDLE_VALUE)
    {
      printf("Failed to create destination file: %s\n", destination_path.c_str());
      CloseHandle(h_source);
      return false;
    }

    char *pch_buffer = new char[BUFFER_SIZE];
    DWORD bytes_read = 0, bytes_written = 0;
    BOOL success;

    while ((success = ReadFile(h_source, pch_buffer, BUFFER_SIZE, &bytes_read, NULL))) 
    {
      if (bytes_read == 0)
      {
        break;
      }
      success = WriteFile(h_destination, pch_buffer, bytes_read, &bytes_written, NULL);
      if (!success || bytes_written != bytes_read)
      {
        printf("Error writing to destination file: %s\n", destination_path.c_str());
        CloseHandle(h_source);
        CloseHandle(h_destination);
        return false;
      }

    }

    delete[] pch_buffer;
    CloseHandle(h_source);
    CloseHandle(h_destination);
  
  #else 
    int source_fd = open(source_path.c_str(), O_RDONLY);
    if (source_fd < 0)
    {
      printf("Failed to open source file: %s\n", source_path.c_str());
      return false;
    }

    int destination_fd = open(destination_path.c_str(), O_WRONLY | O_CREAT | O_TRUNC);
    if (destination_fd < 0)
    {
      printf("Failed to create destination file: %s\n", destination_path.c_str());
      return false;
    }

    char *pch_buffer = new char[BUFFER_SIZE];
    ssize_t bytes_read = 0, bytes_written = 0;

    while ((bytes_read = read(source_fd, pch_buffer, BUFFER_SIZE)) > 0)
    {
      bytes_written = write(destination_fd, pch_buffer, bytes_read);
      if (bytes_written != bytes_read)
      {
        printf("Error writing to destination file: %s\n", destination_path.c_str());
        close(source_fd);
        close(destination_fd);
        delete[] pch_buffer;
        return false; 
      }
    }

    close(source_fd);
    close(destination_fd);
    delete[] pch_buffer;
  #endif

  return true;
}