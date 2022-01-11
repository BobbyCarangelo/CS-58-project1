// album.c
// CS 58
// sample code to collect input from the user

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <signal.h>   // for kill call
#include <unistd.h>   // for fork call
#include <sys/wait.h> // for waitpid

#define STRING_LEN  32

// prompt the user with message, and save input at buffer
// (which should have space for at least len bytes)
int input_string(char *message, char *buffer, int len) {

  int rc = 0, fetched, lastchar;

  if (NULL == buffer)
    return -1;

  if (message)
    printf("%s: ", message);

  // get the string.  fgets takes in at most 1 character less than
  // the second parameter, in order to leave room for the terminating null.  
  // See the man page for fgets.
  fgets(buffer, len, stdin);
  
  fetched = strlen(buffer);


  // warn the user if we may have left extra chars
  if ( (fetched + 1) >= len) {
    fprintf(stderr, "warning: might have left extra chars on input\n");
    rc = -1;
  }

  // consume a trailing newline
  if (fetched) {
    lastchar = fetched - 1;
    if ('\n' == buffer[lastchar])
      buffer[lastchar] = '\0';
  }

  return rc;
}

/*
// testing out the above code
int main(int argc, char * argv[]) {

  char *buffer;

  buffer = (char *)malloc(STRING_LEN);

  if (NULL == buffer) {
    fprintf(stderr,"malloc error\n");
    return -1;
  }
   

  input_string("enter y/n", buffer,STRING_LEN);
  printf("that was [%s]\n", buffer);

  input_string("enter name", buffer,STRING_LEN);
  printf("that was [%s]\n", buffer);

  free(buffer);

  return 0;
}
*/

void exit_parent(int rc, int pid1, int pid2)
{
	fprintf(stderr, "parent exiting with code %d\n", rc);
	if (pid1 > 0)
		kill(pid1, SIGTERM);
	if (pid2 > 0)
		kill(pid2, SIGTERM);
}

int main (int argc, char *argv[])
{
	int rc = 0;
	int status;
	int pid1 = 0;
	int pid2 = 0;
	bool done = false;

	char *buffer;

	buffer = (char *)malloc(STRING_LEN);

	if (NULL == buffer) {
		fprintf(stderr,"malloc error\n");
		return -1;
	}

	// while (!done)
	// {
	// 	input_string(">>>", buffer,STRING_LEN);
  	// 	printf("that was [%s]\n", buffer);
		
	// }

	pid1 = fork();

	if (pid1 < 0)
	{
		printf("Error in fork\n");
		exit_parent(-1, pid1, pid2);
	}

	if (pid1 == 0)
	{
		printf("Hello from child!\n");
		rc = execlp("./convert", "convert", "-resize", "10%", "med1.jpg", "dest.jpg", NULL);
		printf("Execlp failure with exit code: %d", rc);
	}
	else 
	{
		sleep(5);
		printf("Hello from parent!\n");
		waitpid(pid1, &status, 0);
	}

	return 0;
}

  
