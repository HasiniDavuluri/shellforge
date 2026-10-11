#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "parser.h"
#include "builtin.h"

static int redirect_io(const command_t *cmd)
{
int fd;

if (cmd->input[0] != '\0')
{
    fd = open(cmd->input, O_RDONLY);

    if (fd < 0)
    {
        perror(cmd->input);
        return -1;
    }

    if (dup2(fd, STDIN_FILENO) < 0)
    {
        perror("dup2");
        close(fd);
        return -1;
    }

    close(fd);
}

if (cmd->output[0] != '\0')
{
    int flags = O_WRONLY | O_CREAT;

    flags |= cmd->append ? O_APPEND : O_TRUNC;

    fd = open(cmd->output, flags, 0644);

    if (fd < 0)
    {
        perror(cmd->output);
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) < 0)
    {
        perror("dup2");
        close(fd);
        return -1;
    }

    close(fd);
}

return 0;

}

int execute_pipeline(pipeline_t *pipeline)
{
if (pipeline == NULL || pipeline->command_count <= 0)
return -1;

/*
 * Execute a single built-in in the parent process.
 * This is important for commands such as cd.
 */
if (pipeline->command_count == 1 &&
    is_builtin(&pipeline->commands[0]))
{
    int result = execute_builtin(&pipeline->commands[0]);

    return result;
}

int previous_read = -1;
pid_t pids[MAX_COMMANDS];

for (int i = 0; i < pipeline->command_count; i++)
{
    int pipefd[2] = {-1, -1};

    if (i < pipeline->command_count - 1)
    {
        if (pipe(pipefd) < 0)
        {
            perror("pipe");
            if (previous_read != -1)
                close(previous_read);
            return -1;
        }
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");

        if (previous_read != -1)
            close(previous_read);
        if (pipefd[0] != -1)
            close(pipefd[0]);
        if (pipefd[1] != -1)
            close(pipefd[1]);

        return -1;
    }

    if (pid == 0)
    {
        command_t *cmd = &pipeline->commands[i];

        if (previous_read != -1)
        {
            if (dup2(previous_read, STDIN_FILENO) < 0)
            {
                perror("dup2");
                _exit(1);
            }
        }

        if (pipefd[1] != -1)
        {
            if (dup2(pipefd[1], STDOUT_FILENO) < 0)
            {
                perror("dup2");
                _exit(1);
            }
        }

        if (previous_read != -1)
            close(previous_read);
        if (pipefd[0] != -1)
            close(pipefd[0]);
        if (pipefd[1] != -1)
            close(pipefd[1]);

        if (redirect_io(cmd) < 0)
            _exit(1);

        if (is_builtin(cmd))
        {
            int result = execute_builtin(cmd);
            _exit(result == 0 ? 0 : 1);
        }

        execvp(cmd->argv[0], cmd->argv);

        perror(cmd->argv[0]);
        _exit(127);
    }

    pids[i] = pid;

    if (previous_read != -1)
        close(previous_read);

    if (pipefd[1] != -1)
        close(pipefd[1]);

    previous_read = pipefd[0];
}

if (previous_read != -1)
    close(previous_read);

int status = 0;

if (!pipeline->commands[0].background)
{
    for (int i = 0; i < pipeline->command_count; i++)
    {
        int child_status;

        if (waitpid(pids[i], &child_status, 0) < 0)
        {
            perror("waitpid");
            status = -1;
        }
        else if (WIFEXITED(child_status))
        {
            status = WEXITSTATUS(child_status);
        }
        else if (WIFSIGNALED(child_status))
        {
            status = 128 + WTERMSIG(child_status);
        }
    }
}

return status;

}
