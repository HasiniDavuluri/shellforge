#include <stdio.h>
#include <string.h>
#include "parser.h"

static void command_init(command_t *cmd)
{
    cmd->argc = 0;
    cmd->input[0] = '\0';
    cmd->output[0] = '\0';
    cmd->append = 0;
    cmd->background = 0;

    for (int i = 0; i < MAX_ARGS; i++)
        cmd->argv[i] = NULL;
}

int parser(token_list_t *tokens, pipeline_t *pipeline)
{
    if (tokens == NULL || pipeline == NULL)
        return 0;

    pipeline->command_count = 1;
    int current = 0;

    command_init(&pipeline->commands[current]);

    for (int i = 0; i < tokens->count; i++)
    {
        token_t *t = &tokens->tokens[i];
        command_t *cmd = &pipeline->commands[current];

        switch (t->type)
        {
            case TOKEN_WORD:
                if (cmd->argc >= MAX_ARGS - 1)
                {
                    printf("Parser Error: too many arguments.\n");
                    return 0;
                }

                cmd->argv[cmd->argc++] = t->text;
                break;

            case TOKEN_INPUT:
            case TOKEN_OUTPUT:
            case TOKEN_APPEND:
            {
                if (i + 1 >= tokens->count ||
                    tokens->tokens[i + 1].type != TOKEN_WORD)
                {
                    printf("Parser Error: filename expected after redirection.\n");
                    return 0;
                }

                char *filename = tokens->tokens[++i].text;

                if (filename == NULL ||
                    strlen(filename) >= MAX_FILENAME)
                {
                    printf("Parser Error: invalid or too-long filename.\n");
                    return 0;
                }

                if (t->type == TOKEN_INPUT)
                {
                    strcpy(cmd->input, filename);
                }
                else
                {
                    strcpy(cmd->output, filename);
                    cmd->append = (t->type == TOKEN_APPEND);
                }

                break;
            }

            case TOKEN_BACKGROUND:
                cmd->background = 1;
                break;

            case TOKEN_PIPE:
                if (cmd->argc == 0)
                {
                    printf("Parser Error: empty command before pipe.\n");
                    return 0;
                }

                if (current + 1 >= MAX_COMMANDS)
                {
                    printf("Parser Error: too many commands in pipeline.\n");
                    return 0;
                }

                cmd->argv[cmd->argc] = NULL;

                current++;
                pipeline->command_count++;

                command_init(&pipeline->commands[current]);
                break;

            case TOKEN_END:
                break;
        }
    }

    command_t *last = &pipeline->commands[current];

    if (last->argc == 0)
    {
        printf("Parser Error: empty command.\n");
        return 0;
    }

    last->argv[last->argc] = NULL;

    return 1;
}

void pipeline_print(const pipeline_t *pipeline)
{
    if (pipeline == NULL)
        return;

    printf("\n========== PIPELINE ==========\n");

    for (int i = 0; i < pipeline->command_count; i++)
    {
        const command_t *cmd = &pipeline->commands[i];

        printf("\nCommand %d\n", i + 1);
        printf("-----------------------------\n");

        for (int j = 0; j < cmd->argc; j++)
            printf("argv[%d] = %s\n", j, cmd->argv[j]);

        printf("Input      : %s\n",
               cmd->input[0] ? cmd->input : "None");

        printf("Output     : %s\n",
               cmd->output[0] ? cmd->output : "None");

        printf("Append     : %s\n", cmd->append ? "Yes" : "No");

        printf("Background : %s\n",
               cmd->background ? "Yes" : "No");
    }

    printf("==============================\n");
}
