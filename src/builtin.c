#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#include "builtin.h"

/* Built-in: cd */
static int builtin_cd(command_t *cmd)
{
    const char *directory;

    if (cmd->argc == 1)
    {
        directory = getenv("HOME");

        if (directory == NULL)
        {
            fprintf(stderr, "cd: HOME not set\n");
            return -1;
        }
    }
    else if (cmd->argc == 2)
    {
        directory = cmd->argv[1];
    }
    else
    {
        fprintf(stderr, "cd: too many arguments\n");
        return -1;
    }

    if (chdir(directory) != 0)
    {
        perror("cd");
        return -1;
    }

    return 0;
}

/* Built-in: pwd */
static int builtin_pwd(command_t *cmd)
{
    char current_directory[4096];

    if (cmd->argc != 1)
    {
        fprintf(stderr, "pwd: too many arguments\n");
        return -1;
    }

    if (getcwd(current_directory, sizeof(current_directory)) == NULL)
    {
        perror("pwd");
        return -1;
    }

    printf("%s\n", current_directory);
    return 0;
}

/* Built-in: echo */
static int builtin_echo(command_t *cmd)
{
    for (int i = 1; i < cmd->argc; i++)
    {
        printf("%s", cmd->argv[i]);

        if (i < cmd->argc - 1)
            printf(" ");
    }

    printf("\n");
    return 0;
}

/* Built-in: exit */
static int builtin_exit(command_t *cmd)
{
    if (cmd->argc > 1)
    {
        fprintf(stderr, "exit: too many arguments\n");
        return -1;
    }

    return 1;
}

/* NEW FEATURE 1: help */
static int builtin_help(command_t *cmd)
{
    (void)cmd;

    printf("\n========== ShellForge Help ==========\n");
    printf("cd        Change directory\n");
    printf("pwd       Print working directory\n");
    printf("echo      Display text\n");
    printf("exit      Exit ShellForge\n");
    printf("help      Display help information\n");
    printf("env       Display environment variables\n");
    printf("which     Locate an executable command\n");
    printf("clear     Clear the terminal screen\n");
    printf("hostname  Display the system hostname\n");
    printf("=====================================\n");

    return 0;
}

/* NEW FEATURE 2: env */
static int builtin_env(command_t *cmd)
{
    extern char **environ;

    if (cmd->argc != 1)
    {
        fprintf(stderr, "env: this version takes no arguments\n");
        return -1;
    }

    for (char **entry = environ; *entry != NULL; entry++)
    {
        printf("%s\n", *entry);
    }

    return 0;
}

/* NEW FEATURE 3: which */
static int builtin_which(command_t *cmd)
{
    if (cmd->argc != 2)
    {
        fprintf(stderr, "Usage: which COMMAND\n");
        return -1;
    }

    const char *name = cmd->argv[1];

    if (strchr(name, '/') != NULL)
    {
        if (access(name, X_OK) == 0)
        {
            printf("%s\n", name);
            return 0;
        }

        fprintf(stderr, "which: %s: not executable or not found\n", name);
        return -1;
    }

    const char *path = getenv("PATH");

    if (path == NULL)
    {
        fprintf(stderr, "which: PATH not set\n");
        return -1;
    }

    char *path_copy = strdup(path);

    if (path_copy == NULL)
    {
        perror("which");
        return -1;
    }

    char *saveptr = NULL;
    char *directory = strtok_r(path_copy, ":", &saveptr);

    while (directory != NULL)
    {
        char candidate[4096];

        int length = snprintf(candidate, sizeof(candidate),
                              "%s/%s", directory, name);

        if (length >= 0 &&
            (size_t)length < sizeof(candidate) &&
            access(candidate, X_OK) == 0)
        {
            printf("%s\n", candidate);
            free(path_copy);
            return 0;
        }

        directory = strtok_r(NULL, ":", &saveptr);
    }

    free(path_copy);
    fprintf(stderr, "which: %s not found\n", name);
    return -1;
}

/* NEW FEATURE 4: clear */
static int builtin_clear(command_t *cmd)
{
    if (cmd->argc != 1)
    {
        fprintf(stderr, "clear: no arguments expected\n");
        return -1;
    }

    printf("\033[2J\033[H");
    fflush(stdout);

    return 0;
}

/* NEW FEATURE 5: hostname */
static int builtin_hostname(command_t *cmd)
{
    char name[256];

    if (cmd->argc != 1)
    {
        fprintf(stderr, "hostname: no arguments expected\n");
        return -1;
    }

    if (gethostname(name, sizeof(name)) != 0)
    {
        perror("hostname");
        return -1;
    }

    name[sizeof(name) - 1] = '\0';
    printf("%s\n", name);

    return 0;
}

/* Check whether a command is built in */
int is_builtin(const command_t *cmd)
{
    if (cmd == NULL || cmd->argc == 0 || cmd->argv[0] == NULL)
        return 0;

    const char *name = cmd->argv[0];

    return strcmp(name, "cd") == 0 ||
           strcmp(name, "pwd") == 0 ||
           strcmp(name, "echo") == 0 ||
           strcmp(name, "exit") == 0 ||
           strcmp(name, "help") == 0 ||
           strcmp(name, "env") == 0 ||
           strcmp(name, "which") == 0 ||
           strcmp(name, "clear") == 0 ||
           strcmp(name, "hostname") == 0;
}

/* Execute a built-in command */
int execute_builtin(command_t *cmd)
{
    if (cmd == NULL || cmd->argc == 0 || cmd->argv[0] == NULL)
        return -1;

    const char *name = cmd->argv[0];

    if (strcmp(name, "cd") == 0)
        return builtin_cd(cmd);

    if (strcmp(name, "pwd") == 0)
        return builtin_pwd(cmd);

    if (strcmp(name, "echo") == 0)
        return builtin_echo(cmd);

    if (strcmp(name, "exit") == 0)
        return builtin_exit(cmd);

    if (strcmp(name, "help") == 0)
        return builtin_help(cmd);

    if (strcmp(name, "env") == 0)
        return builtin_env(cmd);

    if (strcmp(name, "which") == 0)
        return builtin_which(cmd);

    if (strcmp(name, "clear") == 0)
        return builtin_clear(cmd);

    if (strcmp(name, "hostname") == 0)
        return builtin_hostname(cmd);

    return -1;
}
