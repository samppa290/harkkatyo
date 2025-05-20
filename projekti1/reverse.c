#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    // Open files if given
    FILE *output;
    FILE *input;

    if (argc == 3)
    {
        output = fopen(argv[2], "w");
    }
    else
    {
        output = stdout;
    }

    if (argc > 1)
    {
        input = fopen(argv[1], "r");
    }
    else
    {
        input = stdin;
    }

    // define starting memory
    int numberOfLines = 5;
    char **lines = malloc(numberOfLines * sizeof(char *));

    //Variables
    char *line = NULL;
    size_t len = 0;
    int lineCount = 0;

    // IF TOO MANY ARGUMENTS
    if (argc > 3)
    {
        fprintf(stderr, "usage: reverse <input> <output>");
        exit(1);
    }

    if (argc >= 3)
    {
        // INPUT SAME AS OUTPUT (ChatGPT helped with strcmp function)
        if (strcmp(argv[1], argv[2]) == 0)
        {
            fprintf(stderr, "Input and output file must differ");
            exit(1);
        }
    }

    // INPUT FILE OPENING FAIL
    if (input == NULL)
    {
        fprintf(stderr, "error: cannot open file %s\n", argv[1]);
        exit(1);
    }

    // MALLOC FAIL
    if (lines == NULL)
    {
        fprintf(stderr, "malloc failed");
        exit(1);
    }

    while (getline(&line, &len, input) != -1)
    {

        // Remove newline character (ChatGPT helped with strcspn function)
        line[strcspn(line, "\n")] = '\0';

        //Increase malloc if needed
        if (lineCount >= numberOfLines)
        {
            numberOfLines++;
            char **temp = realloc(lines, numberOfLines * sizeof(char *));
            lines = temp;
        }

        // Allocate memory for each line
        lines[lineCount] = malloc(strlen(line) + 1);
        strcpy(lines[lineCount], line);
        lineCount++;
    }

    //Print data to output reversed
    for (int i = lineCount - 1; i >= 0; i--)
    {
        fprintf(output, "%s\n", lines[i]);
    }

    fclose(input);
    fclose(output);
    return 0;
}
