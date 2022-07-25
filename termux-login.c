#include <stdio.h>

#define BUFSIZE 1024
#define PWDFILELOCATION "/data/data/com.termux/files/usr/etc/termux-login-pwd"

int inputandparse(char buf[]);
int writefile(void);
int readfileandparse(char user[], char host[], char pass[]);
int loginprompt(char user[], char host[], char pass[]);

int main(void)
{
	char user[BUFSIZE];
	char host[BUFSIZE];
	char pass[BUFSIZE];
	
	if (readfileandparse(user, host, pass) && loginprompt(user, host, pass))
	{
		return 0;
	}
	else
	{
		if (writefile() && readfileandparse(user, host, pass) && loginprompt(user, host, pass))
		{
			return 0;
		}
	}
	
	return 1;
}

int inputandparse(char buf[])
{
	while (1)
	{
		if (fgets(buf, BUFSIZE, stdin))
		{
			buf[strcspn(buf, "\n")] = 0;
			if (buf[0] == '\0')
			{
				fprintf(stderr, "String is empty.\nPlease try again: ");
			}
			else
			{
				if (strchr(buf, ':'))
				{
					fprintf(stderr, "String contains ':'.\nI don't know how to escape this, sorry.\nDon't use ':' until I figure out how to escape it.\nPlease try again: ");
				}
				else
				{
					return 1;
				}
			}
		}
		else
		{
			fprintf(stderr, "fgets failed in inputandparse().\n");
			return 0;
		}
	}
}

int writefile(void)
{
	char buf[BUFSIZE];
	FILE *pwdfile = fopen(PWDFILELOCATION, "w");
	
	printf("Creating new termux-login-pwd file.\n");
	if (pwdfile)
	{
		printf("Enter new username: ");
		if (inputandparse(buf))
		{
			fprintf(pwdfile, "%s:", buf);
		}
		else
		{
			fclose(pwdfile);
			return 0;
		}
		printf("Enter new hostname: ");
		if (inputandparse(buf))
		{
			fprintf(pwdfile, "%s:", buf);
		}
		else
		{
			fclose(pwdfile);
			return 0;
		}
		printf("Enter new password: ");
		if (inputandparse(buf))
		{
			fprintf(pwdfile, "%s\n", buf);
			fclose(pwdfile);
			return 1;
		}
		else
		{
			fclose(pwdfile);
			return 0;
		}
	}
	else
	{
		fprintf(stderr, "fopen writefile failed.\n");
		return 0;
	}
}

int readfileandparse(char user[], char host[], char pass[])
{
	char buf[BUFSIZE];
	char *token;
	FILE *pwdfile = fopen(PWDFILELOCATION, "r");
	
	if (pwdfile)
	{
		if (fgets(buf, BUFSIZE, pwdfile))
		{
			buf[strcspn(buf, "\n")] = 0;
			token = strtok(buf, ":");
			if (token)
			{
				strcpy(user, token);
			}
			else
			{
				fprintf(stderr, "Username parsing failed.\n");
				fclose(pwdfile);
				return 0;
			}
			token = strtok(NULL, ":");
			if (token)
			{
				strcpy(host, token);
			}
			else
			{
				fprintf(stderr, "Hostname parsing failed.\n");
				fclose(pwdfile);
				return 0;
			}
			token = strtok(NULL, ":");
			if (token)
			{
				strcpy(pass, token);
				fclose(pwdfile);
				return 1;
			}
			else
			{
				fprintf(stderr, "Passname parsing failed.\n");
				fclose(pwdfile);
				return 0;
			}
		}
		else
		{
			fprintf(stderr, "fgets failed in readfileandparse().\n");
			fclose(pwdfile);
			return 0;
		}
	}
	else
	{
		fprintf(stderr, "fopen readfileandparse failed.\n");
		return 0;
	}
}

int loginprompt(char user[], char host[], char pass[])
{
	char buf[BUFSIZE];
	int usermatch = 0;
	int passmatch = 0;
	
	printf("termux-login made by johnsmithgit143\n\n");
	
	while (!usermatch || !passmatch)
	{
		printf("%s login: ", host);
		if (inputandparse(buf))
		{
			if (strcmp(buf, user) == 0)
			{
				usermatch = 1;
			}
		}
		else
		{
			return 0;
		}
		printf("Password: ");
		if (inputandparse(buf))
		{
			if (strcmp(buf, pass) == 0)
			{
				passmatch = 1;
			}
			else
			{
				printf("\nIncorrect password.\n\n");
			}
		}
		else
		{
			return 0;
		}
	}
	printf("\nSuccessful login.\n");
	return 1;
}
