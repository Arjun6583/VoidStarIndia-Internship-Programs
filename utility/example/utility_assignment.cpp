#include <stdio.h>
#include "vsi_function.h"
#define ONE_KB 1024

bool readWriteFileOperation()
{ 
    INT errorCode = 0;
    /*
        Reding and writing file operation
    */
    HANDLE fileHandle = vsiCreateFileA("/media/sf_Internship_Program/data/source.txt", GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, &errorCode);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        printf("Error opening file for reading: %d\n", errorCode);
        return false;
    }

    printf("File opened successfully for reading.\n");

    HANDLE writeHandle = vsiCreateFileA("/media/sf_Internship_Program/data/destination.txt", GENERIC_WRITE, FILE_SHARE_WRITE, CREATE_ALWAYS, &errorCode);

    char *pchBuffer = new char[ONE_KB];
    PLONG totalBytesRead = new long(0);
    PLONG totalBytesWritten = new long(0);

    while(vsiReadFile(fileHandle, pchBuffer, ONE_KB, totalBytesRead, &errorCode))
    {
        if (!vsiWriteFile(writeHandle, pchBuffer, *totalBytesRead, totalBytesWritten, &errorCode))
        {
            printf("Error writing to file: %d\n", errorCode);
            delete[] pchBuffer;
            delete totalBytesRead;
            delete totalBytesWritten;
            vsiCloseFile(fileHandle);
            vsiCloseFile(writeHandle);
            return false;
        }
        if (*totalBytesWritten != *totalBytesRead)
        {
            printf("Error: Bytes written do not match bytes read.\n");
            delete[] pchBuffer;
            delete totalBytesRead;
            delete totalBytesWritten;
            vsiCloseFile(fileHandle);
            vsiCloseFile(writeHandle);
            return false;
        }  
    }
    return true;
}

bool renameFileOperation()
{
    int errorCode = 0;
    BOOL result;
    result = vsiRenameFileA("/media/sf_Internship_Program/data/copy_source.txt", "/media/sf_Internship_Program/data/renamed_copy.txt", &errorCode);
    if (result == false)
    {
        printf("Error renaming file: %d\n", errorCode);
        return false;
    }

    LPCWSTR fileName = L"माझीफाईल.txt";
    int errorNo = 0;
    HANDLE fileHandle = vsiCreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, OPEN_ALWAYS, &errorNo);
    if (fileHandle == INVALID_HANDLE_VALUE)
    {
        printf("Error opening file: %d\n", errorNo);
        return false;
    }
    printf("File opened successfully.\n");
    vsiCloseFile(fileHandle);

    result = vsiRenameFileW(fileName, L"माझीफाईल_नवीन.txt", &errorNo);
    if (result == false)
    {
        printf("Error renaming file: %d\n", errorNo);
        vsiCloseFile(fileHandle);
        return false;
    }
    printf("File renamed successfully.\n");
    return true;
}

bool deleteFileOperation()
{
    int errorCode = 0;
    BOOL result;
    result = vsiDeleteFileA("/media/sf_Internship_Program/data/renamed_copy.txt", &errorCode);
    if (result == false)
    {
        printf("Error deleting file: %d\n", errorCode);
        return false;
    }
    printf("File deleted successfully.\n");


    LPCWSTR fileName = L"माझीफाईल_नवीन.txt";
    int errorNo = 0;
    result = vsiDeleteFileW(fileName, &errorNo);
    if (result == false)
    {
        printf("Error deleting file: %d\n", errorNo);
        return false;
    }
    printf("File deleted successfully.\n");
    return true;
}

int main()
{
    int errorCode = 0;
    bool result = readWriteFileOperation();
    if (!result)
    {
        printf("Error in read/write file operation.\n");
        return false;
    }
    printf("Read/Write file operation completed successfully.\n");

    BOOL result = vsiCopyFile( "/media/sf_Internship_Program/data/source.txt", "/media/sf_Internship_Program/data/copy_source.txt", &errorCode);
    if (result == false)
    {
        printf("Erro copying file: %d\n", errorCode);
        return -1;
    }
    printf("File copied successfully.\n");

    result = renameFileOperation();
    if (!result)
    {
        printf("Error in rename file operation.\n");
        return -1;
    }

    printf("Rename file operation completed successfully.\n");

    result = deleteFileOperation();
    if (!result)
    {
        printf("Error in delete file operation.\n");
        return -1;
    }

    printf("Delete file operation completed successfully.\n");
    
    return 1;
}