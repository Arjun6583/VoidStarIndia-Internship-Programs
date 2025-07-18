#include <stdio.h>
#include "vsi_function.h"
#define ONE_KB 1024

bool readWriteFileOperation()
{
    int errorCode = 0;
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

    char *buffer = new char[ONE_KB];
    PLONG totalBytesRead = new long(0);
    PLONG totalBytesWritten = new long(0);

    while(vsiReadFile(fileHandle, buffer, ONE_KB, totalBytesRead, &errorCode))
    {
        if (!vsiWriteFile(writeHandle, buffer, *totalBytesRead, totalBytesWritten, &errorCode))
        {
            printf("Error writing to file: %d\n", errorCode);
            delete[] buffer;
            delete totalBytesRead;
            delete totalBytesWritten;
            vsiCloseFile(fileHandle);
            vsiCloseFile(writeHandle);
            return false;
        }
        if (*totalBytesWritten != *totalBytesRead)
        {
            printf("Error: Bytes written do not match bytes read.\n");
            delete[] buffer;
            delete totalBytesRead;
            delete totalBytesWritten;
            vsiCloseFile(fileHandle);
            vsiCloseFile(writeHandle);
            return false;
        }  
    }
    return true;
}

int main()
{
    int errorCode = 0;
    bool result = readWriteFileOperation();
    if (!result)
    {
        printf("Error in read/write file operation.\n");
        return -1;
    }
    printf("Read/Write file operation completed successfully.\n");

    /*
        Copy file operation
    */
    BOOL result = vsiCopyFile( "/media/sf_Internship_Program/data/source.txt", "/media/sf_Internship_Program/data/copy_source.txt", &errorCode);
    if (result == false)
    {
        printf("Erro copying file: %d\n", errorCode);
        return -1;
    }
    printf("File copied successfully.\n");

    /*
        Rename file operation
    */
    result = vsiRenameFile("/media/sf_Internship_Program/data/copy_source.txt", "/media/sf_Internship_Program/data/renamed_copy.txt", &errorCode);
    if (result == false)
    {
        printf("Error renaming file: %d\n", errorCode);
        return -1;
    }
    printf("File renamed successfully.\n");

    /*
        Delete file operation
    */
    result = vsiDeleteFile("/media/sf_Internship_Program/data/renamed_copy.txt", &errorCode);
    if (result == false)
    {
        printf("Error deleting file: %d\n", errorCode);
        return -1;
    }
    printf("File deleted successfully.\n");

    return 1;
}