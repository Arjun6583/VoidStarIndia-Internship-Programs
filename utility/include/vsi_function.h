#pragma once 

#include <stdio.h>

#ifdef _WIN32
  #include <windows.h>
  #include <io.h>

#else 
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/stat.h>
  #include <sys/file.h>

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
#define OPEN_EXISTING 0  /
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

#ifndef FILE_ATTRIBUTE_TEMPORARY
#define FILE_ATTRIBUTE_TEMPORARY 0  
#endif

#ifndef FILE_FLAG_DELETE_ON_CLOSE
#define FILE_FLAG_DELETE_ON_CLOSE 0  
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

  #ifndef LPWORD
    #define LPWORD   long*
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
#endif

HANDLE vsi_create_file(LPCSTR filePath,
                     DWORD accessMode,
                     DWORD shareMode,
                     DWORD creationMode);

BOOL vsi_write_file(HANDLE &file, 
                    LPCVOID buffer, 
                    DWORD numberOfBytesTOWrite);

BOOL vsi_read_file(HANDLE &hFile, 
                  LPVOID buffer, 
                  DWORD numberOfBytesToRead);

void vsi_close_file(HANDLE &hFile);

BOOL vsi_seek_file(HANDLE &hFile, 
                  LONG lOffset, 
                  DWORD origin);