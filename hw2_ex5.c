#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

const char *delimiters = "\t\n ";
const int max_task_len = 1001;
const int max_file_name = 100;
const int max_cmd_num = 500;
const int max_args_num = 25;

typedef struct {
	char *command;
	char **command_args;
	int num_of_args, time_to_sleep;
} task;

/* Parses the strings containing the description of the current command 
   Description structure: time to wait before executing -> command -> arguments of the command*/

void split(char *string, const char *delimiters, char ***tokens,
	   int *tokens_count)
{
	char *token;
	int tokens_size = 0, next_size = 4, cnt = 0;
	char **buff_tokens = *tokens;
	for (token = strtok(string, delimiters); token != NULL;
	     token = strtok(NULL, delimiters)) {
		if (cnt == tokens_size) {
			tokens_size = next_size;
			next_size *= 2;
			buff_tokens =
			    (char **)realloc(buff_tokens,
					     tokens_size * sizeof(char *));
		}
		buff_tokens[cnt++] = token;
	}
	*tokens = buff_tokens;
	*tokens_count = cnt;
}

/* Forms the list of the commands to be executed */

void get_tasks(FILE * file, int num, task ** tasks)
{
	int i;
	char **buff_args = (char **)malloc((max_args_num + 1) * sizeof(char *));
	for (i = 0; i < num; ++i) {
		(*tasks)[i].command_args =
		    (char **)malloc(max_args_num * sizeof(char));
		char *current_task =
		    (char *)malloc(max_task_len * sizeof(char));
		fgets(current_task, max_task_len, file);
		split(current_task, delimiters, &(buff_args),
		      &((*tasks)[i].num_of_args));
		(*tasks)[i].time_to_sleep = atoi(buff_args[0]);
		(*tasks)[i].command = buff_args[1];
		(*tasks)[i].num_of_args--;
		int j;
		for (j = 0; j < (*tasks)[i].num_of_args; ++j)
			(*tasks)[i].command_args[j] = buff_args[j + 1];
		(*tasks)[i].command_args[j + 1] = NULL;
	}
	free(buff_args);

}

/* Executes the commands according to the formed list */

void execute_tasks(task ** tasks, int num)
{
	pid_t pid = 0;
	int i, status;
	for (i = 0; i < num; ++i) {
		pid = fork();
		if (pid == 0) {
			pid_t id = getpid();
			printf("Command to execute: %s\nProcess id: %d\n\n",
			       (*tasks)[i].command, id);
			sleep((*tasks)[i].time_to_sleep);
			execvp((*tasks)[i].command, (*tasks)[i].command_args);
			printf("Error\n");
			exit(-1);
		}
	}
	for (i = num; i > 0; i--)
		pid = wait(&status);
}

/* Frees the memory allocated for the list */

void destroy_tasks(task ** tasks, int num)
{
	int i;
	for (i = 0; i < num; ++i)
		free((*tasks)[i].command_args);
	free(*tasks);
}

int main()
{
	char *file_name = (char *)malloc(max_file_name * sizeof(char));
	printf("Please, enter the name of the input file\n");
	gets(file_name);
	FILE *infile = fopen(file_name, "r");
	char *cmd_num = (char *)malloc(max_task_len * sizeof(char));
	fgets(cmd_num, max_task_len, infile);
	int commands_num = atoi(cmd_num), i;

	task *exec_tasks = (task *) malloc(max_cmd_num * sizeof(task));
	get_tasks(infile, commands_num, &(exec_tasks));
	printf("\n");
	execute_tasks(&(exec_tasks), commands_num);

	destroy_tasks(&(exec_tasks), commands_num);
	free(cmd_num);
	free(file_name);
	fclose(infile);
	return 0;
}
