#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "lexer.h"

void lexer(const char *input,
           token_list_t *list)
{
    token_list_init(list);

    int i = 0;

    while (input[i] != '\0')
    {
        /* Skip whitespace */
        if (isspace((unsigned char)input[i]))
        {
            i++;
            continue;
        }

        /* Logical AND: && */
        if (input[i] == '&' && input[i + 1] == '&')
        {
            token_add(list, TOKEN_AND, "&&");
            i += 2;
            continue;
        }

        /* Logical OR: || */
        if (input[i] == '|' && input[i + 1] == '|')
        {
            token_add(list, TOKEN_OR, "||");
            i += 2;
            continue;
        }

        /* Pipe: | */
        if (input[i] == '|')
        {
            token_add(list, TOKEN_PIPE, "|");
            i++;
            continue;
        }

        /* Here Document: << */
        if (input[i] == '<' && input[i + 1] == '<')
        {
            token_add(list, TOKEN_HEREDOC, "<<");
            i += 2;
            continue;
        }

        /* Input redirection: < */
        if (input[i] == '<')
        {
            token_add(list, TOKEN_INPUT, "<");
            i++;
            continue;
        }

        /* Output redirection: > and >> */
        if (input[i] == '>')
        {
            if (input[i + 1] == '>')
            {
                token_add(list, TOKEN_APPEND, ">>");
                i += 2;
            }
            else
            {
                token_add(list, TOKEN_OUTPUT, ">");
                i++;
            }

            continue;
        }

        /* Error output redirection: 2> */
        if (input[i] == '2' && input[i + 1] == '>')
        {
            token_add(list, TOKEN_ERROR_OUTPUT, "2>");
            i += 2;
            continue;
        }

        /* Background: & */
        if (input[i] == '&')
        {
            token_add(list, TOKEN_BACKGROUND, "&");
            i++;
            continue;
        }

        /* Command separator: ; */
        if (input[i] == ';')
        {
            token_add(list, TOKEN_SEPARATOR, ";");
            i++;
            continue;
        }

        /* Build one WORD token */

        char word[MAX_TOKEN_LEN];

        int j = 0;

        while (input[i] != '\0')
        {
            char c = input[i];

            /* End of word */
            if (isspace((unsigned char)c) ||
                c == '|' ||
                c == '<' ||
                c == '>' ||
                c == '&' ||
                c == ';')
            {
                break;
            }

            /* Single Quote */

            if (c == '\'')
            {
                i++;

                while (input[i] != '\0' && input[i] != '\'')
                {
                    if (j < MAX_TOKEN_LEN - 1)
                        word[j++] = input[i];

                    i++;
                }

                /* Unterminated single quote */

                if (input[i] != '\'')
                {
                    printf("Lexer Error : Unterminated single quote\n");
                    return;
                }

                i++;
                continue;
            }

            /* Double Quote */

            if (c == '"')
            {
                i++;

                while (input[i] != '\0' && input[i] != '"')
                {
                    /* Escaped character inside "" */

                    if (input[i] == '\\' && input[i + 1] != '\0')
                    {
                        i++;
                    }

                    if (j < MAX_TOKEN_LEN - 1)
                        word[j++] = input[i];

                    i++;
                }

                /* Unterminated double quote */

                if (input[i] != '"')
                {
                    printf("Lexer Error : Unterminated double quote\n");
                    return;
                }

                i++;
                continue;
            }

            /* Escape Character */

            if (c == '\\')
            {
                i++;

                if (input[i] != '\0')
                {
                    if (j < MAX_TOKEN_LEN - 1)
                        word[j++] = input[i];

                    i++;
                }

                continue;
            }

            /* Normal Character */

            if (j < MAX_TOKEN_LEN - 1)
                word[j++] = c;

            i++;
        }

        word[j] = '\0';

        if (j > 0)
        {
            token_add(list, TOKEN_WORD, word);
        }
    }

    token_add(list, TOKEN_END, "END");
}
