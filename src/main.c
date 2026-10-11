#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "history.h"
#include "token.h"
#include "lexer.h"
#include "parser.h"
#include "expand.h"
#include "builtin.h"
#include "execute.h"

#define SHELL_NAME "ShellForge"
#define MAX_ALIASES 50
#define ALIAS_NAME_SIZE 64
#define ALIAS_VALUE_SIZE 256
#define MAX_LINE_SIZE 4096

typedef struct
{
    char name[ALIAS_NAME_SIZE];
    char value[ALIAS_VALUE_SIZE];
} alias_t;

static alias_t aliases[MAX_ALIASES];
static int alias_count = 0;

static void print_banner(void)
{
    printf("=====================================\n");
    printf("           %s\n", SHELL_NAME);
    printf("     A Unix Style Shell written in C\n");
    printf("=====================================\n");
    printf("Type 'help' for commands or 'exit' to quit.\n\n");
}

static int find_alias(const char *name)
{
    for (int i = 0; i < alias_count; i++)
    {
        if (strcmp(aliases[i].name, name) == 0)
            return i;
    }

    return -1;
}

static void list_aliases(void)
{
    for (int i = 0; i < alias_count; i++)
    {
        printf("alias %s='%s'\n",
               aliases[i].name,
               aliases[i].value);
    }
}

static int handle_alias(char *line)
{
    if (strcmp(line, "alias") == 0)
    {
        list_aliases();
        return 1;
    }

    if (strncmp(line, "alias ", 6) != 0)
        return 0;

    char *assignment = line + 6;

    while (*assignment == ' ' || *assignment == '\t')
        assignment++;

    char *equals = strchr(assignment, '=');

    if (equals == NULL)
    {
        fprintf(stderr, "Usage: alias name='command'\n");
        return 1;
    }

    *equals = '\0';

    char *name = assignment;
    char *value = equals + 1;

    char *name_end = name + strlen(name);

    while (name_end > name &&
           (name_end[-1] == ' ' || name_end[-1] == '\t'))
    {
        *--name_end = '\0';
    }

    size_t value_len = strlen(value);

    if (value_len >= 2 &&
        ((value[0] == '\'' && value[value_len - 1] == '\'') ||
         (value[0] == '"' && value[value_len - 1] == '"')))
    {
        value[value_len - 1] = '\0';
        value++;
    }

    if (*name == '\0' || *value == '\0')
    {
        fprintf(stderr, "alias: invalid name or value\n");
        return 1;
    }

    if (strlen(name) >= ALIAS_NAME_SIZE ||
        strlen(value) >= ALIAS_VALUE_SIZE)
    {
        fprintf(stderr, "alias: name or value is too long\n");
        return 1;
    }

    int index = find_alias(name);

    if (index >= 0)
    {
        snprintf(aliases[index].value,
                 sizeof(aliases[index].value),
                 "%s", value);

        printf("Alias updated: %s\n", name);
        return 1;
    }

    if (alias_count >= MAX_ALIASES)
    {
        fprintf(stderr, "alias: maximum aliases reached\n");
        return 1;
    }

    snprintf(aliases[alias_count].name,
             sizeof(aliases[alias_count].name),
             "%s", name);

    snprintf(aliases[alias_count].value,
             sizeof(aliases[alias_count].value),
             "%s", value);

    alias_count++;

    printf("Alias created: %s\n", name);
    return 1;
}

static char *expand_alias(const char *line)
{
    char current[MAX_LINE_SIZE];

    if (strlen(line) >= sizeof(current))
    {
        fprintf(stderr, "ShellForge: command is too long\n");
        return NULL;
    }

    snprintf(current, sizeof(current), "%s", line);

    for (int depth = 0; depth < 10; depth++)
    {
        char *start = current;

        while (*start == ' ' || *start == '\t')
            start++;

        if (*start == '\0')
            break;

        char *end = start;

        while (*end != '\0' && *end != ' ' && *end != '\t')
            end++;

        size_t word_len = (size_t)(end - start);

        if (word_len == 0 || word_len >= ALIAS_NAME_SIZE)
            break;

        char name[ALIAS_NAME_SIZE];

        memcpy(name, start, word_len);
        name[word_len] = '\0';

        int index = find_alias(name);

        if (index < 0)
            break;

        char next[MAX_LINE_SIZE];

        size_t prefix_len = (size_t)(start - current);
        size_t value_len = strlen(aliases[index].value);
        size_t suffix_len = strlen(end);

        if (prefix_len + value_len + suffix_len >= sizeof(next))
        {
            fprintf(stderr, "ShellForge: expanded command is too long\n");
            return NULL;
        }

        memcpy(next, current, prefix_len);
        next[prefix_len] = '\0';

        strcat(next, aliases[index].value);
        strcat(next, end);

        snprintf(current, sizeof(current), "%s", next);
    }

    size_t length = strlen(current);
    char *result = malloc(length + 1);

    if (result == NULL)
    {
        perror("malloc");
        return NULL;
    }

    memcpy(result, current, length + 1);
    return result;
}

int main(void)
{
    token_list_t tokens;
    pipeline_t pipeline;

    print_banner();

    while (1)
    {
        char *line = readline("shellforge$ ");

        if (line == NULL)
        {
            printf("\nGoodbye!\n");
            break;
        }

        if (line[0] == '\0')
        {
            free(line);
            continue;
        }

        if (handle_alias(line))
        {
            free(line);
            continue;
        }

        if (strcmp(line, "history") == 0)
        {
            print_history();
            free(line);
            continue;
        }

        add_history(line);

        char *expanded_line = expand_alias(line);
        free(line);

        if (expanded_line == NULL)
            continue;

        lexer(expanded_line, &tokens);

        if (!parser(&tokens, &pipeline))
        {
            fprintf(stderr, "ShellForge: unable to parse command.\n");
            free(expanded_line);
            continue;
        }

        expand_variables(&pipeline);

        int result = execute_pipeline(&pipeline);

        if (pipeline.command_count == 1 &&
            pipeline.commands[0].argc > 0 &&
            pipeline.commands[0].argv[0] != NULL &&
            strcmp(pipeline.commands[0].argv[0], "exit") == 0 &&
            is_builtin(&pipeline.commands[0]) &&
            result == 1)
        {
            free(expanded_line);
            printf("Exiting ShellForge...\n");
            break;
        }

        free(expanded_line);
    }

    return 0;
}

