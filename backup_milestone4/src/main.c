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

/* Display the shell banner */
static void print_banner(void)
{
printf("=====================================\n");
printf("           %s\n", SHELL_NAME);
printf("     A Unix Style Shell written in C\n");
printf("=====================================\n");
printf("Type 'exit' to quit the shell.\n\n");
}

int main(void)
{
token_list_t tokens;
pipeline_t pipeline;

print_banner();

while (1)
{
    /* Read a command from the user */
    char *line = readline("shellforge$ ");

    /* Handle Ctrl+D or end-of-input */
    if (line == NULL)
    {
        printf("\nGoodbye!\n");
        break;
    }

    /* Ignore empty input */
    if (line[0] == '\0')
    {
        free(line);
        continue;
    }

    /* Handle the history command */
    if (strcmp(line, "history") == 0)
    {
        print_history();
        free(line);
        continue;
    }

    /* Save command in Readline history */
    add_history(line);

    /* Milestone 2: Tokenize the input */
    lexer(line, &tokens);

    /* Display tokens for debugging */
    token_print(&tokens);

    /* Parse the tokens */
    if (!parser(&tokens, &pipeline))
    {
        fprintf(stderr,
                "ShellForge: unable to parse command.\n");

        free(line);
        continue;
    }

    /* Expand environment variables */
    expand_variables(&pipeline);

    /* Milestone 4: Execute built-ins and external commands */
    int result = execute_pipeline(&pipeline);

    /*
     * The built-in exit command returns 1.
     * Check the command name so an external command's
     * exit status cannot accidentally close the shell.
     */
    if (pipeline.command_count == 1 &&
        pipeline.commands[0].argc > 0 &&
        pipeline.commands[0].argv[0] != NULL &&
        strcmp(pipeline.commands[0].argv[0], "exit") == 0 &&
        is_builtin(&pipeline.commands[0]) &&
        result == 1)
    {
        free(line);
        printf("Exiting ShellForge...\n");
        break;
    }

    free(line);
}

return 0;

}
