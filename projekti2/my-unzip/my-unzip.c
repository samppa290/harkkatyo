#include <stdio.h>
#include <stdlib.h>

void uncompress(FILE *inputFile)
{
    int count = 1;
    char ch;

    //Used ChatGPT to finish the while loop
    while (fread(&count, sizeof(int), 1, inputFile) == 1)
    {
        fread(&ch, sizeof(char), 1, inputFile);
        for (int i = 0; i < count; i++)
        {
            printf("%c", ch);
        }
    }
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
            printf("my-unzip: cannot open file\n");
            exit(1);
        }

        uncompress(inputFile);
        fclose(inputFile);
    }

    exit(0);
}