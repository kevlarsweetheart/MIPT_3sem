#include <sys/types.h>

struct mymsgbuf
{
	long mtype;
	pid_t pid;
	int x, y;
};

struct mymsgbuf2
{
	long mtype;
	int res;
};

struct thread_data
{
	int msqid, sem_id, x, y;
	pid_t pid;
};
