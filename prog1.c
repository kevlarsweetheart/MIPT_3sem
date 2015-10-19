#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

const int max_prog_len =  100000;

int main()
{
  key_t key = ftok("prog1.c", 0);
  printf("key = %d\n", key);

  int shm_id = shmget(key, max_prog_len, IPC_CREAT | 0666);
  printf("shm_Id: %d\n", shm_id);

  char *data = shmat(shm_id, 0, 0);

  int file_desc = open("prog1.c", O_RDONLY);
  char string[max_prog_len];
  int i = 0;

  while(read(file_desc, &string[i], sizeof(char)) > 0)
  {
    i++;
  }

  strcpy(data, string);
  close(file_desc);
  return 0;
}
