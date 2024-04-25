#include "microshell.h"

void ft_puterror(char *s1, char *s2)
{
	int i = 0;
	while (s1[i])
		i++;
	write(STDERR_FILENO, s1, i);
	if (s2)
	{
		i = 0;
		while (s2[i])
			i++;
		write(STDERR_FILENO, s2, i);
	}
	write(STDERR_FILENO, "\n", 1);
	exit(1);
}

void ft_exec(int ac, char **av, char **ev, int tmpfd)
{
	av[ac] = NULL;
	dup2(tmpfd, STDIN_FILENO);
	close(tmpfd);
	execve(av[0], av, ev);
	ft_puterror("error: cannot execute ", av[0]);
	exit(1);
}

int main(int ac, char **av, char **ev)
{
	int i = 0;
	int tmpfd = dup(STDOUT_FILENO);

	while (av[i] && av[i + 1])
	{
		av = &av[i + 1];

		ac = 0;
		while (av[ac] && strcmp(av[ac], "|") && strcmp(av[ac], ";"))
			ac++;
		i = ac;

		if (strcmp(av[0], "cd") == 0)
		{
			if (ac != 2)
				ft_puterror("error: cd: bad arguments", NULL);
			else if (chdir(av[1]) != 0)
				ft_puterror("error: cd: cannot change directory to ", av[1]);
		}
		else if (ac != 0 && (av[ac] == NULL || strcmp(av[ac], ";") == 0))
		{
			if (fork() == 0)
				ft_exec(ac, av, ev, tmpfd);
			else
			{
				close(tmpfd);
				while (waitpid(-1, NULL, WUNTRACED) != -1);
				tmpfd = dup(STDOUT_FILENO);
			}
		}
		else if(ac != 0 && strcmp(av[ac], "|") == 0)
		{
			int fd[2];
			pipe(fd);
			if (fork() == 0)
			{
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
				ft_exec(ac, av, ev, tmpfd);
			}
			else
			{
				close(fd[1]);
				close(tmpfd);
				tmpfd = fd[0];
			}
		}
	}
	close(tmpfd);
	return (0);
}