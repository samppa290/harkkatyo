#include <stdio.h>
#include <stdlib.h>

void compress(FILE *inputFile)
{
    int currentChar, nextChar;
    int count = 1;

    currentChar = fgetc(inputFile);

    if (currentChar == EOF)
        return;

    while ((nextChar = fgetc(inputFile)) != EOF)
    {

        if (nextChar == currentChar)
        {
            count++;
        }
        else
        {
            // ChatGPT
            // Write count (4 bytes) and character (1 byte)
            fwrite(&count, sizeof(int), 1, stdout);
            fwrite(&currentChar, sizeof(char), 1, stdout);

            currentChar = nextChar;
            count = 1;
        }
    }

    fwrite(&count, sizeof(int), 1, stdout);
    fwrite(&currentChar, sizeof(char), 1, stdout);

    
}

int main(int argc, char *argv[])
{

    FILE *inputFile;

    if (argc == 1)
    {
        printf("my-unzip: file1 [file2 ...]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        inputFile = fopen(argv[i], "r");

        if (inputFile == NULL)
        {
            printf("my-zip: cannot open file\n");
            exit(1);
        }

        compress(inputFile);
        fclose(inputFile);
    }
    exit(0);
}