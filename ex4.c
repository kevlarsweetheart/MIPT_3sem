#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *delimiters = ",. ?!():-/;#$%& \t\n";

void Split(char *string, char *delimiters, char ***tokens, int *tokensCount)
{
	char *token;
	int tokens_size = 0, next_size = 4, cnt = 0;
	char **buff_tokens = *tokens;
	for (token = strtok(string, delimiters); token != NULL; token = strtok(NULL, delimiters))
	{
		if (*tokensCount == tokens_size)
		{
			tokens_size = next_size;
			next_size *= 2;
			buff_tokens = (char **)realloc(buff_tokens, tokens_size * sizeof(char *));
		}
		buff_tokens[cnt++] = token;
	}
	*tokens = buff_tokens;
	*tokensCount = cnt;
}

int main()
{
	char *str = (char *)malloc(sizeof(char));
	gets(str);
	char **tokens = NULL;
	int tokensCount = 0;
	Split(str, delimiters, &tokens, &tokensCount);
	int i;
	printf("The string contains %d tokens\n", tokensCount);
	printf("The tokens:\n");
	for (i = 0; i < tokensCount; ++i)
		printf("%s\n", tokens[i]);
	return 0;
}
