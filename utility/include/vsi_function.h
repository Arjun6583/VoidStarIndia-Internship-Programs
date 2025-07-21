#pragma once 

#include <stdio.h>
#include <iostream>



#ifdef _WIN32
  #include <windows.h>
  #include <io.h>
  #define PLONG long*
#elif defined(__linux__)
  #include <error.h>
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/stat.h>
  #include <sys/file.h> 
  typedef void *PVOID;
  #define PLONG long*
  #ifndef HANDLE 
    #define HANDLE int
  #endif

  #ifndef INVALID_HANDLE_VALUE
    #define INVALID_HANDLE_VALUE -1
  #endif


  #ifndef DWORD 
    #define DWORD unsigned long
  #endif

  #ifndef BOOL
    #define BOOL bool
  #endif

#ifndef GENERIC_READ
#define GENERIC_READ O_RDONLY
#endif

#ifndef GENERIC_WRITE
#define GENERIC_WRITE O_WRONLY
#endif

#ifndef FILE_SHARE_READ
#define FILE_SHARE_READ 0  
#endif

#ifndef FILE_SHARE_WRITE
#define FILE_SHARE_WRITE 0  
#endif

#ifndef CREATE_NEW
#define CREATE_NEW (O_CREAT | O_EXCL)
#endif

#ifndef CREATE_ALWAYS
#define CREATE_ALWAYS (O_CREAT | O_TRUNC)
#endif

#ifndef OPEN_EXISTING
#define OPEN_EXISTING 0  
#endif

#ifndef OPEN_ALWAYS
#define OPEN_ALWAYS O_CREAT
#endif

#ifndef TRUNCATE_EXISTING
#define TRUNCATE_EXISTING O_TRUNC
#endif

#ifndef FILE_ATTRIBUTE_NORMAL
#define FILE_ATTRIBUTE_NORMAL 0  
#endif

#ifndef FILE_FLAG_SEQUENTIAL_SCAN
#define FILE_FLAG_SEQUENTIAL_SCAN POSIX_FADV_SEQUENTIAL  
#endif

#ifndef FILE_FLAG_OVERLAPPED
#define FILE_FLAG_OVERLAPPED 0  
#endif

  #ifndef FILE_BEGIN
    #define FILE_BEGIN    SEEK_SET
  #endif

  #ifndef FILE_CURRENT
    #define FILE_CURRENT  SEEK_CUR
  #endif

  #ifndef FILE_END
    #define FILE_END      SEEK_END
  #endif

  #ifndef LONG
    #define LONG     long
  #endif

  #ifndef PLONG
    #define PLONG    long*
  #endif

  #ifndef LPCVOID
    #define LPCVOID  const void*
  #endif

  #ifndef LPVOID
    #define LPVOID   void*
  #endif

  #ifndef LPCSTR
    #define LPCSTR   const char*
  #endif

  #ifndef LPWORD
    #define LPWORD    WORD*
  #endif  

  #ifndef ULONG_PTR
    #define ULONG_PTR unsigned long
  #endif

  #ifndef VOID
    #define VOID void
  #endif

  #ifndef LPCWSTR
    #define LPCWSTR const wchar_t*
  #endif

  #ifndef INT
    #define INT int
  #endif

  #ifndef PINT
    #define PINT int*
  #endif

#endif


HANDLE vsiCreateFileW(LPCWSTR lpFileName,
                     DWORD dwDesiredAccess,
                     DWORD dwShareMode,
                     DWORD dwCreationDisposition, 
                     PINT piErrorCode = nullptr);

HANDLE vsiCreateFileA(LPCSTR lpFileName,
                     DWORD dwDesiredAccess,
                     DWORD dwShareMode,
                     DWORD dwCreationDisposition, 
                     PINT ipErrorCode = nullptr);

BOOL vsiWriteFile(HANDLE &file, 
                    LPCVOID buffer, 
                    DWORD numberOfBytesTOWrite, 
                    PLONG totalBytesWritten, 
                    PINT ipErrorCode = nullptr);

BOOL vsiReadFile(HANDLE &hFile, 
                  LPVOID buffer, 
                  DWORD numberOfBytesToRead, 
                  PLONG totalBytesRead, 
                  PINT ipErrorCode = nullptr);

VOID vsiCloseFile(HANDLE &hFile);

BOOL vsiSeekFile(HANDLE &hFile, 
                  LONG lOffset, 
                  DWORD origin, PINT piErrorCode = nullptr);

BOOL vsiCopyFile(LPCSTR sourcelpFileName, 
                  LPCSTR destinationlpFileName, 
                  PINT piErrorCode = nullptr);

BOOL vsiRenameFileA(LPCSTR oldlpFileName, 
                  LPCSTR newlpName, 
                  PINT ipErrorCode = nullptr);

BOOL vsiRenameFileW(LPCWSTR oldlpFileName,
                   LPCWSTR newlpName,
                   PINT ipErrorCode = nullptr);

BOOL vsiDeleteFileA(LPCSTR lpFileName, PINT ipErrorCode = nullptr);

BOOL vsiDeleteFileW(LPCWSTR lpFileName, PINT ipErrorCode = nullptr);

