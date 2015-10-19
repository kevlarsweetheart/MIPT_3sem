#include <stdio.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

const int max_prog_len = 100000;

int main()
{
  key_t key = ftok("prog1.c", 0);
  printf("key = %d\n", key);

  int shm_id = shmget(key, max_prog_len, 0666);
  printf("shm_Id: %d\n", shm_id);

  char *data = shmat(shm_id, 0, 0);
  char string[max_prog_len];
  strcpy(string, data);
  printf("%s", string);
  shmctl(shm_id, IPC_RMID, NULL);
  return 0;
}
