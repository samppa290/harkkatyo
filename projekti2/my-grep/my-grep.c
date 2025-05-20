#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{

    FILE *inputFile;
    char *TextToGrep;

    // IF NO COMMAND LINE ARGUMENTS
    if (argc == 1)
    {
        fprintf(stderr, "my-grep: searchterm [file ...]\n");
        exit(1);
    }

    // Set text to find 

    TextToGrep = argv[1];
    

    // buffer
    char buffer[1024];

    // if only search term given, read from stdin
    if (argc == 2)
    {
        while (fgets(buffer, sizeof(buffer), stdin))
        {
            if (strstr(buffer, TextToGrep) != NULL)
            {
                printf("%s", buffer);
            }

            // exit if user enters an empty string
            if (strcmp(buffer, "\n") == 0)
            {
                exit(0);
            }
        }
        return 0;
    }


    // loop multiple files
    for (int i = 2; i < argc; i++)
    {

        inputFile = fopen(argv[i], "r");
        if (inputFile == NULL)
        {
            printf("cannot Open File\n");
            exit(1);
        }

        // Loop each file
        while (fgets(buffer, sizeof(buffer), inputFile))
        {
            // if line includes wanted word print, help from https://stackoverflow.com/questions/12784766/check-substring-exists-in-a-string-in-c
            if (strstr(buffer, TextToGrep) != NULL)
            {
                printf("%s", buffer);
            }
        }
        fclose(inputFile);

        printf("\n");
    }

    return 0;
}