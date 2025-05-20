#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{

    FILE *inputFile;

    // exit if no files given
    if (argc == 1)
    {
        exit(0);
    }

    // buffer
    char buffer[1024];

    // loop multiple files
    for (int i = 1; i < argc; i++)
    {
        
        if (inputFile == NULL)
        {
            printf("my-cat: cannot open file\n");
            exit(1);
        }

        inputFile = fopen(argv[i], "r");
        
        // Loop each file
        while (fgets(buffer, sizeof(buffer), inputFile))
        {
            printf("%s", buffer);
        }
        fclose(inputFile);

        printf("\n");
    }

    exit(0);
}