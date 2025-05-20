#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int paths_count = 0;
char *paths[999];

void run_cd(char **args, int arg_count)
{
    if (arg_count != 2)
    {
        fprintf(stderr, "An error has occurred\n");
        return;
    }
    // change the directory
    if (chdir(args[1]) != 0)
    {
        fprintf(stderr, "An error has occurred\n");
    }
}

void run_exit(char **args, int arg_count)
{
    if (arg_count != 1)
    {
        fprintf(stderr, "An error has occurred\n");
    }
    else
    {
        exit(0);
    }
}

void run_path(char **args, int arg_count)
{
    for (int i = 0; i < paths_count; i++)
    {
        free(paths[i]);
    }
    paths_count = 0;
    for (int i = 1; i < arg_count; i++)
    {
        paths[paths_count++] = strdup(args[i]);
    }
}

// executes external commands
void exec_external(char **args, bool redirect, int arg_count)
{
    char complete_path[1024];
    char *output = NULL;

    // detect redirection
    if (redirect)
    {
        if (arg_count < 3 || strcmp(args[arg_count - 2], ">") != 0)
        {
            fprintf(stderr, "An error has occurred\n");
            exit(1);
        }
        output = args[arg_count - 1];
        args[arg_count - 2] = NULL; //terminate args before >
    }

    for (int i = 0; i < paths_count; i++)
    {
        // chatGPT used to format snprintf
        snprintf(complete_path, sizeof(complete_path), "%s/%s", paths[i], args[0]);
        if (access(complete_path, X_OK) == 0)
        {
            if (output)
            {
                int fd = open(output, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                if (fd < 0)
                {
                    fprintf(stderr, "An error has occurred\n");
                    exit(1);
                }
                dup2(fd, STDOUT_FILENO);
                dup2(fd, STDERR_FILENO);
                close(fd);
            }
            execv(complete_path, args);
            // if execv returns its error
            fprintf(stderr, "An error has occurred\n");
            exit(1);
        }
    }

    // no executable
    fprintf(stderr, "An error has occurred\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    FILE *input = stdin;

    // batch mode setup
    if (argc > 2)
    {
        fprintf(stderr, "An error has occurred\n");
        return 1;
    }
    if (argc == 2)
    {
        input = fopen(argv[1], "r");
        if (input == NULL)
        {
            fprintf(stderr, "An error has occurred\n");
            return 1;
        }
    }

    // default path
    paths[paths_count++] = strdup("/bin");

    while (1)
    {
        if (input == stdin)
        {
            printf("wish> ");
            fflush(stdout);
        }

        read = getline(&line, &len, input);
        if (read == -1)
            break;  // EOF or error

        // strip newline
        line[strcspn(line, "\n")] = '\0';

        // split at & for parallel commands
        // can have up to 64 pararrel commands
        char *parallel_cmds[64];
        int number_commands = 0;
        char *saveptr1;
        char *part = strtok_r(line, "&", &saveptr1);
        while (part && number_commands < 64)
        {
            // trim leading spaces
            while (*part == ' ') part++;
            // trim trailing spaces, chat GPT helped to format this
            char *end = part + strlen(part) - 1;
            while (end > part && *end == ' ') *end-- = '\0';
            if (*part != '\0')
                parallel_cmds[number_commands++] = part;
            part = strtok_r(NULL, "&", &saveptr1);
        }

        pid_t child_pids[64];
        int launched = 0;

        for (int i = 0; i < number_commands; i++)
        {
            // parse each sub command into args
            char *args[1024];
            int arg_count = 0;
            bool redirect = false;

            char *saveptr2;
            char *command = strtok_r(parallel_cmds[i], " ", &saveptr2);
            while (command && arg_count < 1023)
            {
                args[arg_count++] = command;
                command = strtok_r(NULL, " ", &saveptr2);
            }
            args[arg_count] = NULL;
            if (arg_count == 0)
                continue;

            // detect redirection
            if (arg_count >= 3 && strcmp(args[arg_count - 2], ">") == 0)
                redirect = true;

            // built in commands
            if (strcmp(args[0], "cd") == 0)
            {
                if (number_commands > 1)
                    fprintf(stderr, "An error has occurred\n");
                else
                    run_cd(args, arg_count);
                continue;
            }
            if (strcmp(args[0], "exit") == 0)
            {
                if (number_commands > 1)
                    fprintf(stderr, "An error has occurred\n");
                else
                    run_exit(args, arg_count);
                continue;
            }
            if (strcmp(args[0], "path") == 0)
            {
                if (number_commands > 1)
                    fprintf(stderr, "An error has occurred\n");
                else
                    run_path(args, arg_count);
                continue;
            }

            // external command fork
            pid_t pid = fork();
            if (pid < 0)
            {
                perror("fork");
                continue;
            }
            if (pid == 0)
            {
                exec_external(args, redirect, arg_count);
                exit(1);
            }
            else
            {
                child_pids[launched++] = pid;
            }
        }

        // wait for external commands
        for (int i = 0; i < launched; i++)
        {
            waitpid(child_pids[i], NULL, 0);
        }
    }

    free(line);

    if (input != stdin)
        fclose(input);
    return 0;
}
