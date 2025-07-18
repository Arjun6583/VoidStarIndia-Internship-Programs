#include "vsi_function.h"
#include <stdio.h>
#include <iostream>
#define ONE_KB 1024

HANDLE vsiCreateFileW(LPCWSTR lpFileName,
                     DWORD dwDesiredAccess,
                     DWORD dwShareMode,
                     DWORD dwCreationDisposition, int *errorCode)
{
  #ifdef _WIN32
    
    HANDLE hFile = CreateFileW(
      lpFileName,
      dwDesiredAccess,
      dwShareMode,
      NULL,
      dwCreationDisposition,
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

    int fd = open(lpFileName, dwDesiredAccess | dwCreationDisposition, 0644);
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

HANDLE vsiCreateFileA(LPCSTR lpFileName,
                     DWORD dwDesiredAccess,
                     DWORD dwShareMode,
                     DWORD dwCreationDisposition, int *errorCode)
{
#ifdef _WIN32
  
  HANDLE hFile = CreateFileA(
    lpFileName,
    dwDesiredAccess,
    dwShareMode,
    NULL,
    dwCreationDisposition,
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

  int fd = open(lpFileName, dwDesiredAccess | dwCreationDisposition, 0644);

  if (dwShareMode != 0 && flock(fd, dwShareMode) == -1) 
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

BOOL vsiReadFile(HANDLE &hFile, LPVOID buffer, DWORD numberOfBytesToRead, PLONG totalBytesRead, int *errorCode
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

BOOL vsiWriteFile(HANDLE &hFile, LPCVOID buffer, DWORD numberOfBytesToWrite, PLONG totalBytesWritten, int *errorCode)
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

void vsiCloseFile(HANDLE &hFile)
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

BOOL vsi_copy_file(LPCSTR sourcelpFileName,
                  LPCSTR destinationPath,
                  int *errorCode)
{
  #ifdef _WIN32
  HANDLE hSourceFile = vsiCreateFileA(sourcelpFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING,
                                        errorCode);
  if (hSourceFile == INVALID_HANDLE_VALUE) 
  {
    return false;
  }

  HANDLE hDestinationFile = vsiCreateFileA(destinationPath, GENERIC_WRITE, 0, CREATE_ALWAYS,
                                             errorCode);
  if (hDestinationFile == INVALID_HANDLE_VALUE)
  {
    vsiCloseFile(hSourceFile);
    return false;
  }

  char *buffer = new char[ONE_KB];
  PLONG totalBytesRead = new long(0);
  PLONG totalBytesWritten = new long(0);

  while (vsiReadFile(hSourceFile, buffer, ONE_KB, totalBytesRead, errorCode))
  {
    if (!vsiWriteFile(hDestinationFile, buffer, *totalBytesRead, totalBytesWritten, errorCode)) 
    {
      delete[] buffer;
      delete totalBytesRead;
      delete totalBytesWritten;
      vsiCloseFile(hSourceFile);
      vsiCloseFile(hDestinationFile);
      return false;
    }

    if (*totalBytesWritten != *totalBytesRead) 
    {
      delete[] buffer;
      delete totalBytesRead;
      delete totalBytesWritten;
      vsiCloseFile(hSourceFile);
      vsiCloseFile(hDestinationFile);
      return false;
    }
  }
  delete[] buffer;
  delete totalBytesRead;
  delete totalBytesWritten;
  vsiCloseFile(hSourceFile);
  vsiCloseFile(hDestinationFile);

  #elif defined(__linux__)
  int sourcFd = open(sourcelpFileName, O_RDONLY);
  if (sourcFd < 0) 
  {
    return false;
  }
  int destFd = open(destinationPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (destFd < 0) 
  {
    vsiCloseFile(sourcFd);
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
      vsiCloseFile(sourcFd);
      vsiCloseFile(destFd);
      return false;
    }
    if (bytesWritten != bytesRead) 
    {
      if (errorCode != nullptr) 
      {
        *errorCode = errno;
      }
      delete[] buffer;
      vsiCloseFile(sourcFd);
      vsiCloseFile(destFd);
      return false;
    }
  }

  delete [] buffer;
  vsiCloseFile(sourcFd);
  vsiCloseFile(destFd);
  #endif
  return true;
}


BOOL vsiRenameFile(LPCSTR oldlpFileName,
                   LPCSTR newlpName,
                   int *errorCode)
{
  #ifdef _WIN32
    if (MoveFileA(oldlpFileName, newlpName) == 0) 
    {
      *errorCode = GetLastError();
      return false;
    }
  #elif defined(__linux__)
    if (rename(oldlpFileName, newName) != 0) 
    {
      *errorCode = errno;
      return false;
    }
  #endif
  return true;
}

BOOL vsiDeleteFile(LPCSTR lpFileName, int *errorCode)
{
  #ifdef _WIN32
    if (DeleteFileA(lpFileName) == 0)
    {
      if (errorCode != nullptr)
      {
        *errorCode = GetLastError();
      }
      return false;
    }
  #elif defined(__linux__)
    if (unlink(lpFileName) != 0)
    {
      *errorCode = errno;
      return false;
    }
  #endif
  return true;
}