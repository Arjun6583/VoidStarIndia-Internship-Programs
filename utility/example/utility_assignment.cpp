#include <stdio.h>
#include "vsi_function.h"
#define BUFFER_SIZE 1024

int main()
{
    HANDLE hFile = vsi_create_file("E:\\arjun\\Programming\\C ++\\Practice\\Internship Program\\data\\arjun.txt", GENERIC_READ, 0, OPEN_EXISTING);
    if (hFile == INVALID_HANDLE_VALUE) 
    {
        printf("Failed to open file\n");
        return 1;
    }

    HANDLE hFile2 = vsi_create_file("E:\\arjun\\Programming\\C ++\\Practice\\Internship Program\\data\\arjun2.txt", GENERIC_WRITE, 0, CREATE_ALWAYS);
    if (hFile2 == INVALID_HANDLE_VALUE) 
    {
        printf("Failed to open file\n");
        vsi_close_file(hFile);
        return 1;
    }
    LPVOID buffer = new char[BUFFER_SIZE];
    while (vsi_read_file(hFile, buffer, BUFFER_SIZE))
    {
        if (!vsi_write_file(hFile2, buffer, BUFFER_SIZE)) 
        {
            printf("Failed to write to file\n");
            vsi_close_file(hFile);
            vsi_close_file(hFile2);
            return 1;
        }
    }
    delete[] (char *)buffer;
    vsi_close_file(hFile);
    vsi_close_file(hFile2);
    printf("File copied successfully\n");
}