#include "vsi_function.h"
#include <stdio.h>
#include <iostream>
#define ONE_KB 1024

HANDLE vsiCreateFileW(LPCWSTR lpFileName,
                     DWORD dwDesiredAccess,
                     DWORD dwShareMode,
                     DWORD dwCreationDisposition, PINT  ipCodeError)
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
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = GetLastError();
      }
      return INVALID_HANDLE_VALUE;
    }
    return hFile;
  #else
    const wchar_t* widePath = lpFileName;
    char utf8Path[1024];

    setlocale(LC_ALL, "");
    size_t res = wcstombs(utf8Path, widePath, sizeof(utf8Path));
    if (res == (size_t)-1) 
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
      }
      return false;
    }

    int fd = open(utf8Path, dwDesiredAccess | dwCreationDisposition, 0644);
    if (fd < 0) 
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
      }
      return INVALID_HANDLE_VALUE;
    }
    return fd;
  #endif
}

HANDLE vsiCreateFileA(LPCSTR lpFileName,
                     DWORD dwDesiredAccess,
                     DWORD dwShareMode,
                     DWORD dwCreationDisposition, PINT  ipCodeError)
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
    if (ipCodeError != nullptr) 
    {
      *ipCodeError = GetLastError();
    }
    return INVALID_HANDLE_VALUE;
  }

  return hFile;

#else
  int flags = 0;

  int fd = open(lpFileName, dwDesiredAccess | dwCreationDisposition, 0644);

  if (dwShareMode != 0 && flock(fd, dwShareMode) == -1) 
  {
    if (ipCodeError != nullptr) 
    {
      *ipCodeError = errno;
    }
    close(fd);
    return INVALID_HANDLE_VALUE;
  }

  return fd;
  
#endif
}

BOOL vsiReadFile(HANDLE &hFile, LPVOID buffer, DWORD numberOfBytesToRead, PLONG totalBytesRead, PINT  ipCodeError
 )
{
  #ifdef _WIN32
    DWORD bytesRead = 0;
    BOOL result = ReadFile(hFile, buffer, numberOfBytesToRead, &bytesRead, NULL);
    if (result == false) 
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = GetLastError();
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
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
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

BOOL vsiWriteFile(HANDLE &hFile, LPCVOID buffer, DWORD numberOfBytesToWrite, PLONG totalBytesWritten, PINT  ipCodeError)
{
  #ifdef _WIN32
    DWORD bytesWritten = 0;
    BOOL result = WriteFile(hFile, buffer, numberOfBytesToWrite, &bytesWritten, NULL);
    
    if (result == false)
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = GetLastError();
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
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
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

BOOL vsiSeekFile(HANDLE &hFile, LONG lOffset, DWORD origin, PINT  ipCodeError)
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
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = GetLastError();
      }
      return false;
    }

  #elif defined(__linux__)

    off_t newPosition = lseek(hFile, lOffset, origin);
    if (newPosition == -1) 
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
      }
      return false;
    }

  #endif
  return true;
}

BOOL vsi_copy_file(LPCSTR sourcelpFileName,
                  LPCSTR destinationPath,
                  PINT  ipCodeError)
{
  #ifdef _WIN32
  HANDLE hSourceFile = vsiCreateFileA(sourcelpFileName, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING,
                                        ipCodeError);
  if (hSourceFile == INVALID_HANDLE_VALUE) 
  {
    return false;
  }

  HANDLE hDestinationFile = vsiCreateFileA(destinationPath, GENERIC_WRITE, 0, CREATE_ALWAYS,
                                             ipCodeError);
  if (hDestinationFile == INVALID_HANDLE_VALUE)
  {
    vsiCloseFile(hSourceFile);
    return false;
  }

  char *buffer = new char[ONE_KB];
  PLONG totalBytesRead = new long(0);
  PLONG totalBytesWritten = new long(0);

  while (vsiReadFile(hSourceFile, buffer, ONE_KB, totalBytesRead, ipCodeError))
  {
    if (!vsiWriteFile(hDestinationFile, buffer, *totalBytesRead, totalBytesWritten, ipCodeError)) 
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

  char *buffer = new char[ONE_KB];
  ssize_t bytesRead;
  while ((bytesRead = read(sourcFd, buffer, ONE_KB)) > 0)
  {
    ssize_t bytesWritten = write(destFd, buffer, bytesRead);
    if (bytesWritten < 0) 
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
      }
      delete[] buffer;
      vsiCloseFile(sourcFd);
      vsiCloseFile(destFd);
      return false;
    }
    if (bytesWritten != bytesRead) 
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
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


BOOL vsiRenameFileA(LPCSTR oldlpFileName,
                   LPCSTR newlpName,
                   PINT  ipCodeError)
{
  #ifdef _WIN32
    if (MoveFileA(oldlpFileName, newlpName) == 0) 
    {
      *ipCodeError = GetLastError();
      return false;
    }
  #elif defined(__linux__)
    if (rename(oldlpFileName, newlpName) != 0) 
    {
      *ipCodeError = errno;
      return false;
    }
  #endif
  return true;
}

BOOL vsiRenameFileW(LPCWSTR oldlpFileName,
                   LPCWSTR newlpName,
                   PINT  ipCodeError)
{
  #ifdef _WIN32
    if (MoveFileW(oldlpFileName, newlpName) == 0) 
    {
      *ipCodeError = GetLastError();
      return false;
    }
  #elif defined(__linux__)
    const wchar_t* widePath = oldlpFileName;
    char utf8Path[1024];

    setlocale(LC_ALL, "");
    size_t res = wcstombs(utf8Path, widePath, sizeof(utf8Path));
    if (res == (size_t)-1) 
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
      }
      return false;
    }
    const char* oldlpFileNameutf8 = utf8Path;

    res = wcstombs(utf8Path, (const wchar_t *)newlpName, sizeof(utf8Path));
    if (res == (size_t)-1) 
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
      }
      return false;
    }    
    const char* newlpNameutf8 = utf8Path;
    if (rename(oldlpFileNameutf8, newlpNameutf8) != 0) 
    {
      *ipCodeError = errno;
      return false;
    }
  #endif
  return true;
}

BOOL vsiDeleteFileA(LPCSTR lpFileName, PINT  ipCodeError)
{
  #ifdef _WIN32
    if (DeleteFileA(lpFileName) == 0)
    {
      if (ipCodeError != nullptr)
      {
        *ipCodeError = GetLastError();
      }
      return false;
    }
  #elif defined(__linux__)
    if (unlink(lpFileName) != 0)
    {
      *ipCodeError = errno;
      return false;
    }
  #endif
  return true;
}

BOOL vsiDeleteFileW(LPCWSTR lpFileName, PINT  ipCodeError)
{
  #ifdef _WIN32
    if (DeleteFileW(lpFileName) == 0)
    {
      if (ipCodeError != nullptr)
      {
        *ipCodeError = GetLastError();
      }
      return false;
    }
  #elif defined(__linux__)
    const wchar_t* widePath = lpFileName;
    char utf8Path[1024];

    setlocale(LC_ALL, "");
    size_t res = wcstombs(utf8Path, widePath, sizeof(utf8Path));
    if (res == (size_t)-1) 
    {
      if (ipCodeError != nullptr) 
      {
        *ipCodeError = errno;
      }
      return false;
    }

    if (unlink(utf8Path) != 0)
    {
      *ipCodeError = errno;
      return false;
    }
  #endif
  return true;
}