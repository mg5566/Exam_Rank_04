#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define	ERR1	"error: cd: bad arguments\n"
#define	ERR2	"error: cd: cannot change directory to "
#define	ERR3	"error: fatal\n"
#define	ERR4	"error: cannot execute "

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

// argv len
int	ft_argvlen(char *argv[])
{
	int len;

	len = 0;
	while (argv[len])
		len++;
	return (len);
}

// "|" 가 있으면 1 반환, ";" 가 있으면 0 반환
int		is_pipe(char *argv[])
{
	int	idx;

	idx = 0;
	while (argv[idx])
	{
		if (!strncmp(argv[i]. "|", 2))  // "|" 가 있으면
		{
			argv[i] = NULL;  // "|" 자리에 널문자를 넣어줌
			return (1);  // "|" 가 있으면 1반환
		}
		else if (!strncmp(argv[i], ";", 2))
		{
			argv[i] = NULL;  // ";" 자리에 널문자를 넣어줌
			return (0);
		}
		idx++;
	}
	return (0);
}

void	ft_cd(char *argv[])
{
	int	idx;

	idx = ft_argvlen(argv);  // argv 개수
	// argv[0] : "cd"
	// argv[1] : cd 의 인자
	if (idx != 2) // cd 의 인자가 한개가 아니면
		put_error(ERR1);
	else if (chdir(argv[1]) < 0)
	{
		// 디렉토리 변경함수 에러시
		put_error(ERR2);
		put_error(argv[1]);
		put_error("\n");
	}
}

void	cmd(int argc, char *argv[], char *envp[], int prev, int *fd_prev)
{
	pid_t	pid;
	int		fd_next[2];
	int		next;
	int		status;
	int		idx;

	if (argc <= 0)  // 인자가 0이면 바로 종료
		return ;
	next = is_pipe(argv);  // "|" 가 있으면 1, 없으면 0
	// is_pipe 에서 argv[0]가 NULL 이 될 수도 있음.
	if (argv[0])
	{
	}
	idx = ft_argvlen(argv);  // argv 길이
	// 인자는 i+1 만큼 줄고, argv는 i+1만큼 넘어가고, next(파이프가 있는지 없는지), fd_next(파이프가 있으면 pipe된 배열)을 넣어줌
	cmd(argc - (idx + 1), argv + idx + 1, envp, next, fd_next);

}

int main(int argc, char *argv[], char *envp[])
{
	int		fd_prev[2];

	fd_prev[0] = 0;
	fd_prev[1] = 1;
	cmd(argc - 1, argv + 1, envp, 0, fd_prev);
	return (0);
}
