#include <stdlib.h>
#include "parser.h"
#include "expand.h"

void expand_variables(pipeline_t *pipeline)
{
    if (pipeline == NULL) {
        return;
    }

    for (int i = 0; i < pipeline->command_count; i++) {
        command_t *cmd = &pipeline->commands[i];

        for (int j = 0; j < cmd->argc; j++) {
            char *arg = cmd->argv[j];

            if (arg == NULL || arg[0] != '$') {
                continue;
            }

            const char *value = getenv(arg + 1);
            cmd->argv[j] = (char *)(value != NULL ? value : "");
        }
    }
}
