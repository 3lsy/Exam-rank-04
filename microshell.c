#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

void ft_puterror(char *s1, char *s2)
{
	int i = 0;
	while (s1[i])
		i++;
	write(STDERR_FILENO, s1, i);
	i = 0;
	while (s2[i])
		i++;
	write(STDERR_FILENO, s2, i);
	write(STDERR_FILENO, "\n", 1);
}

void	ft_exec(int ac, char **av, char **ev, int tmpfd)
{
	av[ac] = NULL;					// set the last argument to NULL
	dup2(tmpfd, STDIN_FILENO);		// set stdin to tmpfd
	close(tmpfd);					// close tmpfd
	execve(av[0], av, ev);			// execute the cmd
	ft_puterror("error: cannot execute ", av[0]);
	exit(1);
}

int main(int ac, char **av, char **ev)
{
	int i = 0;

	int tmpfd = dup(STDOUT_FILENO);					// save stdout
	while (av[i] && av[i + 1])
	{
		// set av as the cmd to execute
		av = &av[i + 1];							// skip av[0], ; or | (set av to next cmd)
		
	
		// count the cmd arguments
		ac = 0;										// count cmd arguments
		while (av[ac] && strcmp(av[ac], "|") && strcmp(av[ac], ";"))
			ac++;
		i = ac;


		// check if cmd is cd and execute it
		if (strcmp(av[0], "cd") == 0)
		{
			if (ac != 2)
				ft_puterror("error: cd: bad arguments\n", NULL);
			else if (chdir(av[1]) != 0)
				ft_puterror("error: cd: cannot change directory to ", av[1]);
		}


		// check if the cmd is not proceeded by a pipe and execute it on stdout
		else if (ac != 0 && (av[ac] == NULL || strcmp(av[ac], ";") == 0))
		{
			if (fork() == 0)
				ft_exec(ac, av, ev, tmpfd);
			else
			{
				close(tmpfd);
				while(waitpid(-1, NULL, WUNTRACED) != -1);	// wait for child process to finish
				tmpfd = dup(STDOUT_FILENO);					// save stdout
			}
		}


		// check if the cmd is proceeded by a pipe and execute it on the pipe
		else if (ac != 0 && strcmp(av[ac], "|") == 0)
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
