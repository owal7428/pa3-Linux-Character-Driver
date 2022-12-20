#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define RUNNING 1

int main(int argc, char **argv)
{
    char *filename = argv[1];

    if (filename == NULL)
    {
        printf("./pa2test error: missing filename\n");
        return 255;
    }

    //Open file for read/write
    int file = open(filename, O_RDWR);

    if (file == -1)
    {
        printf("./pa2test error: invalid filename\n");
        return 255;
    }

    while (RUNNING)
    {
        printf("Option (r for read, w for write, s for seek): ");
        char option = fgetc(stdin);

        if(option == EOF)
        {
            printf("\n");
            break;
        }
        else if (option == '\n')
            continue;
        
        //Flush extra newline character
        getchar();

        if (option == 'r')
        {
            char input[20];
            int bytes;

            printf("Enter the number of bytes you want to read: ");
            
            //Check for ctrl+D with input
            if (fgets(input, 20, stdin) == NULL)
            {
                printf("\n");
                break;
            }   
            sscanf(input, "%d", &bytes);

            char *output = malloc(bytes);

            read(file, output, bytes);
            printf("%s\n", output);

            free(output);
        }
        else if (option == 'w')
        {
            char *string = malloc(1000);
            int resize = 0;

            printf("Enter the string you want to write: ");

            if (fgets(string, 1000, stdin) == NULL)
            {
                printf("\n");
                free(string);
                break;
            }

            char check = string[0];

            //Find the end of the string so empty space can be deallocated
            while (check != '\0')
            {
                check = string[resize + 1];
                resize++;
            }

            //Get rid of newline character
            string[resize - 1] = '\0';

            //Copy contents of string into a new string and then deallocate
            char *newString = malloc(resize);

            for (int i = 0; i < resize; i++)
            {
                newString[i] = string[i];
            }

            free(string);

            write(file, newString, resize);

            free(newString);
        }
        else if (option == 's')
        {
            char input[20];
            long offset;
            int whence;

            printf("Enter an offset value: ");

            if (fgets(input, 20, stdin) == NULL)
            {
                printf("\n");
                break;
            }
            sscanf(input, "%ld", &offset);

            printf("Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END): ");

            if (fgets(input, 20, stdin) == NULL)
            {
                printf("\n");
                break;
            }
            sscanf(input, "%d", &whence);

            lseek(file, offset, whence);
        }
    }

    close(file);
    
    return 0;
}