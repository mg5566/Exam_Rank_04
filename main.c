#include <unistd.h>
#include <stdlib.h>
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

int	ft_argvlen(char *argv[])
{
	int	len;

	len = 0;
	while (argv[len])
		len++;
	return (len);
}

int	put_error(char *str)
{
	write(2, str, ft_strlen(str));
	return (1);  // 문자열 출력 후 1반환
}

// pipe가 있으면 1, 없으면 0 반환
int		is_pipe(char *argv[])
{
	int	idx;

	idx = 0;
	while (argv[idx])
	{
		if (!strncmp(argv[idx], "|", 2))  // 2칸을 비교해야 정확하게 "|" 만 존재하는 것을 확인할 수 있음
		{
			argv[idx] = NULL;  // "|" 자리에 NULL 을 넣어줌
			return (1);  // "|" 가 잇으면 1을 반환
		}
		else if (!strncmp(argv[idx], ";", 2))  // ";"가 있으면 0 을 반환하시오.
		{
			argv[idx] = NULL;  // ";" 자리에 NULL 을 넣어줍니다.
			return (0);
		}
		idx++;

	}
	return (0);
}

// cd 동작
void	ft_cd(char *argv[])
{
	int	idx;

	idx = ft_argvlen(argv);
	// argv[0] : "cd"
	// argv[1] : cd 의 인자
	if (idx != 2)  // 인자가 2개가 아니면
		put_error(ERR1);
	else if (chdir(argv[1]) < 0)
	{
		// cd 에러시
		put_error(ERR2);
		put_error(argv[1]);
		put_error("\n");
	}
}


// 인자수, 인자, 환경변수, pipe 여부, pipe가 있으면 파이프배열을 넘긴다.
void	cmd(int argc, char *argv[], char *envp[], int prev, int *fd_prev)
{
	pid_t	pid;
	int		fd_next[2];
	int		next, status, idx;

	if (argc <= 0)  // 인자가 0이면 동작 멈춰!
		return ;
	next = is_pipe(argv);  // "|" 가 있으면 1, 없으면 0
	// is_pipe 에서 argv[0]이 NULL 이 될 수도 있음.
	if (argv[0])
	{
		if (next && pipe(fd_next) < 0)  // "|" 가 있을때 pipe 해줌
			exit(put_error(ERR3));
		if (!strncmp(argv[0], "cd", 3))
			ft_cd(argv);  // cd 명령어 처리
		else if ((pid = fork()) < 0)  // cd 가 아닐때 fork 해주고, 프로세스를 만듬
			exit(put_error(ERR3));
		else if (!pid)  // 자식 프로세스
		{
			if (prev && dup2(fd_prev[0], 0) < 0)  // fd_prev[0] 이 stdin 역할
				exit(2);  // 일단 2를 반환하고 종료
			if (next && dup2(fd_next[1], 1) < 0)  // fd_next[1] 이 stdout 역할
				exit(2);  // 일단 2 반환 후 종료
			if (execve(argv[0], argv, envp) < 0)  // execve 함수로 명령어 실행
			{
				// execve 에러
				put_error(ERR4);
				put_error(argv[0]);
				put_error("\n");
				exit(1);
			}
		}
		else  // 부모 프로세스
		{
			if (waitpid(pid, &status, 0) < 0)  // 자식프로세스를 기다림
				exit(put_error(ERR3));
			if (status == 512)  // 자식의 반환값이 512/256 = 2 인 경우 : dup2 에러 exit(2) 상황
				exit(put_error(ERR3));
			// 안사용하는 pipe 닫기
			if (prev)
				close(fd_prev[0]);
			if (next)
				close(fd_next[1]);
		}
	}
	idx = ft_argvlen(argv);  // argv 개수
	// 인자수 i+1만큼 줄이기, argv는 i+1만큼 넘어가기, next(파이프가 있는지 없는지), fd_next(파이프 배열)
	cmd(argc - (idx + 1), argv + idx + 1, envp, next, fd_next);
}

int main(int argc, char *argv[], char *envp[])
{
	int fd_prev[2];

	fd_prev[0] = 0;
	fd_prev[1] = 1;
	cmd(argc - 1, argv + 1, envp, 0, fd_prev);
	return (0);
}
