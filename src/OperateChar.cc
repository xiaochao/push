#include "OperateChar.h"

void SplitChar(char* src, char* dst, int site)
{
	char *result, temp[200];

	strcpy(temp, src);
	result = strtok(temp, "\t");
	while(result != NULL)
	{
		site--;
		if(site == 0)
		{
			strcpy(dst, result);
			break;
		}
		result = strtok(NULL, "\t");
	}
}

vector<string> SplitChar(char *src, const char *sign)
{
	char *result, temp[200], buf[200];
	vector<string> re;

	strcpy(temp, src);
	result = strtok(temp, sign);
	while(result != NULL)
	{
		strcpy(buf, result);
		re.push_back(string(buf));
		result = strtok(NULL, sign);
	}

	return re;
}

void itoa(int i, char *a)
{
	sprintf(a, "%d", i);
}

/*int main(void)
{
	printf(GREEN "hello world\n" NONE);
	return 0;
}*/
