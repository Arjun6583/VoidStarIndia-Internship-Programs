#include "vsi_function.h"
#include <stdio.h>

HANDLE vsi_create_file(LPCSTR filePath,
                     DWORD accessMode,
                     DWORD shareMode,
                     DWORD creationMode)
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

  return hFile;

#else
  int flags = 0;

  int fd = open(filePath, accessMode | creationMode, 0644);

  if (shareMode != 0 && flock(fd, shareMode) == -1) 
  {
    printf("Failed to lock file: %s\n", filePath);
    close(fd);
    return INVALID_HANDLE_VALUE;
  }

  return fd;
  
#endif
}

BOOL vsi_read_file(HANDLE &hFile, LPVOID buffer, DWORD numberOfBytesToRead
 )
{
  #ifdef _WIN32
    DWORD bytesRead = 0;
    BOOL result = ReadFile(hFile, buffer, numberOfBytesToRead, &bytesRead, NULL);
    if (!result) 
    {
      printf("Failed to read file\n");
      return false;
    }
    if (bytesRead == 0) 
    {
      return false;
    }
  #elif defined(__linux__)
    size_t bytesRead = read(hFile, buffer, numberOfBytesToRead);
    if (bytesRead < 0)
    {
      printf("Failed to read file\n");
      return false;
    }
    if (bytesRead == 0) 
    {
      return false;
    }
  #endif
    return true;
}

BOOL vsi_write_file(HANDLE &hFile, LPCVOID buffer, DWORD numberOfBytesToWrite)
{
  #ifdef _WIN32
    DWORD bytesWritten = 0;
    BOOL result = WriteFile(hFile, buffer, numberOfBytesToWrite, &bytesWritten, NULL);
    if (result == false)
    {
      printf("Failed to Write file\n");
      return false;
    }
    if (bytesWritten == 0) 
    {
      return false;
    }
  #elif defined(__linux__)
    size_t bytesWritten = write(hFile, buffer, numberOfBytesToWrite);
    if (bytesWritten < 0)
    {
      printf("Failed to write file\n");
      return false;
    }
    if (bytesWritten != 0) 
    {
      *numberOfBytesWritten = bytesWritten;
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

BOOL vsi_seek_file(HANDLE &hFile, LONG lOffset, DWORD origin)
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
      printf("Failed to seek file\n");
      return false;
    }

  #elif defined(__linux__)

    off_t newPosition = lseek(hFile, lOffset, origin);
    if (newPosition == -1) 
    {
      printf("Failed to seek file\n");
      return false;
    }
    
  #endif
  return true;
}
