#include "vsi_function.h"
#include <stdio.h>
#include <iostream>
#define BUFFER_SIZE 1024

HANDLE vsi_create_file_w(LPCWSTR filePath,
                     DWORD accessMode,
                     DWORD shareMode,
                     DWORD creationMode, int *errorCode)
{
  #ifdef _WIN32
    
    HANDLE hFile = CreateFileW(
      filePath,
      accessMode,
      shareMode,
      NULL,
      creationMode,
      FILE_ATTRIBUTE_NORMAL,
      0
    );
    if (hFile == INVALID_HANDLE_VALUE)
    {
      *errorCode = GetLastError();
      return INVALID_HANDLE_VALUE;
    }
    return hFile;
  #else

    int fd = open(filePath, accessMode | creationMode, 0644);
    if (fd < 0) 
    {
      if (errorCode != nullptr) 
      {
        *errorCode = errno;
      }
      return INVALID_HANDLE_VALUE;
    }
    return fd;
  #endif
}

HANDLE vsi_create_file_a(LPCSTR filePath,
                     DWORD accessMode,
                     DWORD shareMode,
                     DWORD creationMode, int *errorCode)
{
#ifdef _WIN32
  
  HANDLE hFile = CreateFileA(
    filePath,
    accessMode,
    shareMode,
    NULL,
    creationMode,
    FILE_ATTRIBUTE_NORMAL,
    0
  );

  if (hFile == INVALID_HANDLE_VALUE)
  {
    if (errorCode != nullptr) 
    {
      *errorCode = GetLastError();
    }
    return INVALID_HANDLE_VALUE;
  }

  return hFile;

#else
  int flags = 0;

  int fd = open(filePath, accessMode | creationMode, 0644);

  if (shareMode != 0 && flock(fd, shareMode) == -1) 
  {
    if (errorCode != nullptr) 
    {
      *errorCode = errno;
    }
    close(fd);
    return INVALID_HANDLE_VALUE;
  }

  return fd;
  
#endif
}

BOOL vsi_read_file(HANDLE &hFile, LPVOID buffer, DWORD numberOfBytesToRead, PLONG totalBytesRead, int *errorCode
 )
{
  #ifdef _WIN32
    DWORD bytesRead = 0;
    BOOL result = ReadFile(hFile, buffer, numberOfBytesToRead, &bytesRead, NULL);
    if (result == false) 
    {
      if (errorCode != nullptr) 
      {
        *errorCode = GetLastError();
      }
      return false;
    }
    if (bytesRead == 0) 
    {
      return false;
    }
    *totalBytesRead = bytesRead;
  #elif defined(__linux__)
    size_t bytesRead = read(hFile, buffer, numberOfBytesToRead);
    if (bytesRead < 0)
    {
      if (errorCode != nullptr) 
      {
        *errorCode = errno;
      }
      return false;
    }
    if (bytesRead == 0) 
    {
      return false;
    }
  #endif
    return true;
}

BOOL vsi_write_file(HANDLE &hFile, LPCVOID buffer, DWORD numberOfBytesToWrite, PLONG totalBytesWritten, int *errorCode)
{
  #ifdef _WIN32
    DWORD bytesWritten = 0;
    BOOL result = WriteFile(hFile, buffer, numberOfBytesToWrite, &bytesWritten, NULL);
    
    if (result == false)
    {
      if (errorCode != nullptr) 
      {
        *errorCode = GetLastError();
      }
      return false;
    }
    if (bytesWritten == 0) 
    {
      return false;
    }
    *totalBytesWritten = bytesWritten;

  #elif defined(__linux__)
    size_t bytesWritten = write(hFile, buffer, numberOfBytesToWrite);
    if (bytesWritten < 0)
    {
      if (errorCode != nullptr) 
      {
        *errorCode = errno;
      }
      return false;
    }
  #endif 
  return true;
}

void vsi_close_file(HANDLE &hFile)
{
  #ifdef _WIN32
    CloseHandle(hFile);
  #elif defined(__linux__)
    close(hFile);
  #endif
}

BOOL vsi_seek_file(HANDLE &hFile, LONG lOffset, DWORD origin, int *errorCode)
{
  #ifdef _WIN32
    LARGE_INTEGER liDistanceToMove;
    liDistanceToMove.QuadPart = lOffset;
    LARGE_INTEGER liNewPosition;
    BOOL result = SetFilePointerEx(
      hFile, 
      liDistanceToMove, 
      &liNewPosition, 
      origin
    );

    if (result == false)
    {
      if (errorCode != nullptr) 
      {
        *errorCode = GetLastError();
      }
      return false;
    }

  #elif defined(__linux__)

    off_t newPosition = lseek(hFile, lOffset, origin);
    if (newPosition == -1) 
    {
      if (errorCode != nullptr) 
      {
        *errorCode = errno;
      }
      return false;
    }

  #endif
  return true;
}

BOOL vsi_copy_file(LPCSTR sourceFilePath,
                  LPCSTR destinationPath,
                  int *errorCode)
{
  #ifdef _WIN32
  HANDLE hSourceFile = vsi_create_file_a(sourceFilePath, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING,
                                        errorCode);
  if (hSourceFile == INVALID_HANDLE_VALUE) 
  {
    return false;
  }

  HANDLE hDestinationFile = vsi_create_file_a(destinationPath, GENERIC_WRITE, 0, CREATE_ALWAYS,
                                             errorCode);
  if (hDestinationFile == INVALID_HANDLE_VALUE)
  {
    vsi_close_file(hSourceFile);
    return false;
  }

  char *buffer = new char[BUFFER_SIZE];
  PLONG totalBytesRead = new long(0);
  PLONG totalBytesWritten = new long(0);

  while (vsi_read_file(hSourceFile, buffer, BUFFER_SIZE, totalBytesRead, errorCode))
  {
    if (!vsi_write_file(hDestinationFile, buffer, *totalBytesRead, totalBytesWritten, errorCode)) 
    {
      delete[] buffer;
      delete totalBytesRead;
      delete totalBytesWritten;
      vsi_close_file(hSourceFile);
      vsi_close_file(hDestinationFile);
      return false;
    }

    if (*totalBytesWritten != *totalBytesRead) 
    {
      delete[] buffer;
      delete totalBytesRead;
      delete totalBytesWritten;
      vsi_close_file(hSourceFile);
      vsi_close_file(hDestinationFile);
      return false;
    }
  }
  delete[] buffer;
  delete totalBytesRead;
  delete totalBytesWritten;
  vsi_close_file(hSourceFile);
  vsi_close_file(hDestinationFile);

  #elif defined(__linux__)
  int sourcFd = open(sourceFilePath, O_RDONLY);
  if (sourcFd < 0) 
  {
    return false;
  }
  int destFd = open(destinationPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (destFd < 0) 
  {
    close(sourcFd);
    return false;
  }

  char *buffer = new char[BUFFER_SIZE];
  ssize_t bytesRead;
  while ((bytesRead = read(sourcFd, buffer, BUFFER_SIZE)) > 0)
  {
    ssize_t bytesWritten = write(destFd, buffer, bytesRead);
    if (bytesWritten < 0) 
    {
      if (errorCode != nullptr) 
      {
        *errorCode = errno;
      }
      delete[] buffer;
      close(sourcFd);
      close(destFd);
      return false;
    }
    if (bytesWritten != bytesRead) 
    {
      if (errorCode != nullptr) 
      {
        *errorCode = errno;
      }
      delete[] buffer;
      close(sourcFd);
      close(destFd);
      return false;
    }
  }

  delete [] buffer;
  vsi_close_file(sourcFd);
  vsi_close_file(destFd);
  #endif
  return true;
}


BOOL vsi_rename_file(LPCSTR oldFilePath,
                   LPCSTR newName,
                   int *errorCode)
{
  #ifdef _WIN32
    if (MoveFileA(oldFilePath, newName) == 0) 
    {
      *errorCode = GetLastError();
      return false;
    }
  #elif defined(__linux__)
    if (rename(oldFilePath, newName) != 0) 
    {
      *errorCode = errno;
      return false;
    }
  #endif
  return true;
}

BOOL vsi_delete_file(LPCSTR filePath, int *errorCode)
{
  #ifdef _WIN32
    if (DeleteFileA(filePath) == 0)
    {
      if (errorCode != nullptr)
      {
        *errorCode = GetLastError();
      }
      return false;
    }
  #elif defined(__linux__)
    if (unlink(filePath) != 0)
    {
      *errorCode = errno;
      return false;
    }
  #endif
  return true;
}