#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *delimiters = ",. ?!():-/;#$%& \t\n";
const int max_length = 1000000;

void split(char *string, char *delimiters, char ***tokens, int *tokens_count)
{
	char *token;
	int tokens_size = 0, next_size = 4, cnt = 0;
	char **buff_tokens = *tokens;
	for (token = strtok(string, delimiters); token != NULL; token = strtok(NULL, delimiters))
	{
		if (cnt == tokens_size)
		{
			tokens_size = next_size;
			next_size *= 2;
			buff_tokens = (char **)realloc(buff_tokens, tokens_size * sizeof(char *));
		}
		buff_tokens[cnt++] = token;
	}
	*tokens = buff_tokens;
	*tokens_count = cnt;
}

int main()
{
	char *str = (char *)malloc(max_length * sizeof(char));
	gets(str);
	char **tokens = NULL;
	int tokens_count = 0;
	split(str, delimiters, &tokens, &tokens_count);
	int i;
	printf("The string contains %d tokens\n", tokens_count);
	printf("The tokens:\n");
	for (i = 0; i < tokens_count; ++i)
		printf("%s\n", tokens[i]);
	free(str);
	free(tokens);
	return 0;
}
