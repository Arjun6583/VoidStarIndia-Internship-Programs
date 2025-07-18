#include <stdio.h>
#include "vsi_function.h"

int main()
{
    int errorCode = 0;
    BOOL result = vsi_copy_file( "/media/sf_Internship_Program/data/source.txt", "/media/sf_Internship_Program/data/copy_source.txt", &errorCode);
    if (result) 
    {
        printf("File copy successfully.\n");
    } 
    else 
    {
        printf("Failed to copy file. %d\n", errorCode);
        return -1;
    }
    return 1;
}