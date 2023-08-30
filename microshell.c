/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mcreus <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/29 14:33:59 by mcreus            #+#    #+#             */
/*   Updated: 2023/08/29 14:34:05 by mcreus           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

int ft_putstr_fd2(char *str, char *arg)
{
    while (*str)
        write(2, str++, 1);
    if (*arg)
    {
        while (*arg)
            write(2, arg++, 1);
    }
    write(2, "\n", 1);
    return (1);
}

int ft_execute(char **av, int i, int tmp_fd, char **env)
{
    av[i] = NULL;
    dup2(tmp_fd, STDIN_FILENO);
    close(tmp_fd);
    execve(av[0], av, env);
    return (ft_putstr_fd2("error: cannot execute ", av[0]));
}

int main(int ac, char **av, char **env)
{
    int i;
    int tmp_fd;
    int fd[2];
    (void)ac;

    i = 0;
    tmp_fd = dup(STDIN_FILENO);
    while (av[i] && av[i + 1])
    {
        av = &av[i + 1];
        i = 0;
        while (av[i] && strcmp(av[i], ";") && strcmp(av[i], "|"))
            i++;
        if (strcmp(av[0], "cd") == 0)
        {
            if (i != 2)
                ft_putstr_fd2("error: cd: bad arguments", NULL);
            else if (chdir(av[1]) != 0)
                ft_putstr_fd2("error: cd: cannot change directory to ", av[1]);
        }
        else if ((i != 0 && av[i] == NULL) || (i != 0 && strcmp(av[i], ";") == 0))
        {
            if (fork() == 0)
            {
                if (ft_execute(av, i, tmp_fd, env))
                    return (1);
            }
            else
            {
                close(tmp_fd);
                if (waitpid(-1, NULL, WUNTRACED) != -1)
                    ;
                tmp_fd = dup(STDIN_FILENO);
            }
        }
        else if (i != 0 && strcmp(av[i], "|") == 0)
        {
            pipe(fd);
            if (fork() == 0)
            {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
                close(fd[0]);
                if (ft_execute(av, i, tmp_fd, env));
                    return (1);
            }
            else
            {
                close(fd[1]);
                close(tmp_fd);
                tmp_fd = fd[0];
            }
        }
    }
    close(tmp_fd);
    return (0);
}