#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <termios.h>
#include <signal.h>

#define PWDFILEPATH "/data/data/com.termux/files/usr/etc/termux-login-pwd"
#define BUFSIZE 4096

bool prompt(char buf[], const char msg[]);
bool gnu_getpass(char buf[]);
bool createfile(char buf[]);
bool readfile(char buf[], char user[], char host[], char pass[]);
bool login(char buf[], char user[], char host[], char pass[]);

int main(void)
{
	char buf[BUFSIZE];
	char user[BUFSIZE];
	char host[BUFSIZE];
	char pass[BUFSIZE];
	
	signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	
	while (!readfile(buf, user, host, pass))
	{
		createfile(buf);
	}
	if (login(buf, user, host, pass))
		return 0;
	return 1;
}

bool prompt(char buf[], const char msg[])
{
	while (true)
	{
		printf("%s", msg);
		if (fgets(buf, BUFSIZE, stdin))
		{
			buf[strcspn(buf, "\n")] = 0;
			if (buf[0] == '\0')
			{
				fprintf(stderr, "\nMust not be empty.\n");
			}
			else
			{
				if (strchr(buf, ':'))
				{
					fprintf(stderr, "Illegal character ':'.\n");
			 	}
				else return true;
			}
		}
		else return false;
	}
}

bool gnu_getpass(char buf[])
{
    struct termios old, new;
    bool success = false;

    if (tcgetattr(0, &old) != 0)
        return false;
    new = old;
    new.c_lflag &= ~ECHO;
    if (tcsetattr(0, TCSAFLUSH, &new) != 0)
        return false;

    if (prompt(buf, "Password: "))
    	success = true;
    
    (void) tcsetattr(0, TCSAFLUSH, &old);

    return success;
}

bool createfile(char buf[])
{
	FILE *pwdfile = fopen(PWDFILEPATH, "w");
	bool success = true;
	
	printf("Creating new user credentials.\n");
	if (pwdfile)
	{
		if (prompt(buf, "Enter new Username: "))
			fprintf(pwdfile, "%s:", buf);
		else success = false;
		if (prompt(buf, "Enter new Hostname: "))
			fprintf(pwdfile, "%s:", buf);
		else success = false;
		printf("Enter new ");
		if (gnu_getpass(buf))
			fprintf(pwdfile, "%s\n", buf);
		else success = false;
	}
	else	
	{
		fprintf(stderr, "fopen() in createfile() failed.\n");
		return false;
	}
	printf("\n");
	fclose(pwdfile);
	return success;
}

bool readfile(char buf[], char user[], char host[], char pass[])
{
	char *token = NULL;
	bool success = true;
	FILE *pwdfile = fopen(PWDFILEPATH, "r");
	
	if (pwdfile)
	{
		if (fgets(buf, BUFSIZE, pwdfile))
		{
			buf[strcspn(buf, "\n")] = 0;
			token = strtok(buf, ":");
			if (token)
				strcpy(user, token);
			else success = false;
			token = strtok(NULL, ":");
			if (token)
				strcpy(host, token);
			else success = false;
			token = strtok(NULL, ":");
			if (token)
				strcpy(pass, token);
			else success = false;
		}
		else success = false;
	}
	else
	{
		fprintf(stderr, "fopen() in createfile() failed.\n");
		return false;
	}
	fclose(pwdfile);
	if (!success)
	{
		fprintf(stderr, "Error parsing file in readfile()\n");
		return false;
	}
	return true;
}

bool login (char buf[], char user[], char host[], char pass[])
{
	bool usermatch = false;
	bool passmatch = false;
	
	printf("termux-login by johnsmithgit143\n\n");
	
	while (!usermatch || !passmatch)
	{
		usermatch = false;
		passmatch = false;
		printf("%s ", host);
		if (prompt(buf, "login: "))
			usermatch = (strcmp(buf, user) == 0);
		else return false;
		if (gnu_getpass(buf))
			passmatch = (strcmp(buf, pass) == 0);
		else return false;
		if (!usermatch || !passmatch)
			fprintf(stderr, "\n\nIncorrect Password.\n\n");
	}
	printf("\n\nSuccessful Login.\n\n");
	return true;
}
