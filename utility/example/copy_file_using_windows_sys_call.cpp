#include <windows.h>
#include <stdio.h>
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) 
{
    if (argc != 3) 
    {
      printf("arguments: source file path destination file path missing\n");
      return EXIT_FAILURE;
    }

    const char* sourcePath = argv[1];
    const char* destPath = argv[2];

    HANDLE hSource = CreateFileA(
        sourcePath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hSource == INVALID_HANDLE_VALUE) 
    {
        printf("Failed to open source file: %s\n", sourcePath);
        return EXIT_FAILURE;
    }
    if (GetLastError() == 0) 
    {
        printf("Source file newly created: %s\n", sourcePath);
        CloseHandle(hSource);
        return EXIT_FAILURE;
    }

    HANDLE hDest = CreateFileA(
        destPath,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    if (hDest == INVALID_HANDLE_VALUE) 
    {
        printf("Failed to create destination file");
        CloseHandle(hSource);
        return 1;
    }

    char *pch_buffer = new char[BUFFER_SIZE];
    DWORD bytesRead = 0, bytesWritten = 0;
    BOOL success;

    do {
        success = ReadFile(hSource, pch_buffer, BUFFER_SIZE, &bytesRead, NULL);
        if (!success) {
            printf("Error reading from source file");
            break;
        }

        if (bytesRead == 0) 
        {
          break;
        }
          
        success = WriteFile(hDest, pch_buffer, bytesRead, &bytesWritten, NULL);
        if (!success) 
        {
            printf("Error writing to destination file");
            break;
        }

        if (bytesRead != bytesWritten) 
        {
            printf("Mismatch: read %lu bytes, wrote %lu bytes\n", bytesRead, bytesWritten);
            break;
        }

    } while (bytesRead > 0);

    CloseHandle(hSource);
    CloseHandle(hDest);

    if (success) 
    {
        printf("File copied successfully from:\n%s to %s\n", sourcePath, destPath);
    }
    else 
    {
        printf("File copy failed.\n");
    }

    return 0;
}
