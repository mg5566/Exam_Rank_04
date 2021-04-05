#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define ERR1 "error: cd: bad argument\n"
#define ERR2 "error: cd: cannot change directory to "
#define ERR3 "error: fatal\n"
#define ERR4 "error: cannot execute "


int	ft_strlen(char *str)
{
	int	len;

	len = 0;
	while (str[len])
		len++;
	return (len);
}

int	put_error(char *str)
{
	write(2, str, ft_strlen(str));
	return (1);
}

int	ft_argvlen(char *argv[])
{
	int len;

	len = 0;
	while (argv[len])
		len++;
	return (len);
}

// "|" 가 있으면 1, 없으면 0을 반환
int	is_pipe(char *argv[])
{
	int	idx;

	idx = 0;
	while (argv[idx])
	{
		if (!strncmp(argv[idx], "|", 2))
		{
			argv[idx] = NULL;
			return (1);
		}
		else if(!strncmp(argv[idx], ";", 2))
		{
			argv[idx] = NULL;
			return (0);
		}
		idx++;
	}
	return (0);
}

// cd execve 하는 함수
void	ft_cd(char *argv[])
{
	if (ft_argvlen(argv) != 2)  // "cd" "<PATH>" 가 아니면
		put_error(ERR1);
	else if (chdir(argv[1]) < 0)
	{
		put_error(ERR2);
		put_error(argv[1]);
		put_error("\n");
	}
}

void	cmd(int argc, char *argv[], char *envp[], int prev, int *fd_prev)
{
	pid_t	pid;
	int		fd_next[2];
	int		next, status, idx;

	if (argc <= 0)
		return ;
	next = is_pipe(argv);
	if (argv[0])
	{
		if (next && pipe(fd_next) < 0)
			exit(put_error(ERR3));
		if (!strncmp(argv[0], "cd", 3))
			ft_cd(argv);
		else if((pid = fork()) < 0)
			exit(put_error(ERR3));
		else if (!pid)  // 자식
		{
			if (prev && dup2(fd_prev[0], 0) < 0)
				exit(2);
			if (next && dup2(fd_next[1], 1) < 0)
				exit(2);
			if (execve(argv[0], argv, envp) < 0)
			{
				put_error(ERR4);
				put_error(argv[0]);
				put_error("\n");
				exit(1);
			}
		}
		else  // 부모
		{
			if (waitpid(pid, &status, 0) < 0)
				exit(put_error(ERR3));
			if (status == 512)
				exit(put_error(ERR3));
			if (prev)
				close(fd_prev[0]);
			if (next)
				close(fd_next[1]);
		}
	}
	idx = ft_argvlen(argv);
	cmd(argc - (idx + 1), argv + (idx + 1), envp, next, fd_next);
}

int main(int argc, char *argv[], char *envp[])
{
	int	fd_prev[2];

	fd_prev[0] = 0;
	fd_prev[1] = 1;
	cmd(argc - 1, argv + 1, envp, 0, fd_prev);
	return (0);
}
