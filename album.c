/**
 * @file album.c
 * @author Bobby Carangelo
 * @brief Photo album program -- display, edit and caption jpgs. 
 * 		  Demonstrates proper use of fork() and exec() to manage processes.
 * 
 * @version 0.1
 * @date 2022-01-11
 * 
 */

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

void exit_parent(int rc, int pid1, int pid2)
{
	fprintf(stderr, "parent exiting with code %d\n", rc);
	if (pid1 > 0)
		kill(pid1, SIGTERM);
	if (pid2 > 0)
		kill(pid2, SIGTERM);
}

/**
 * @brief Display user options
 * 
 */
void display_menu()
{
	printf("Options:\n");
	printf("  'd' to display an image\n");
	printf("  't' to generate a thumbnail\n");
	printf("  'r' to rotate an image\n");
	printf("  'c' to caption an image\n");
	printf("  'q' to quit\n");
}


/**
 * @brief creates child process that runs image magick display program
 * 
 * @param pic name of picture to be displayed -- must end in .jpg (doesn't check for this)
 * @return int -- process id of child process, -1 on error  
 */
int execute_display(char *pic)
{
	int pid1 = 0;
	int rc = 0;

	pid1 = fork();

	if (pid1 < 0)
	{
		printf("error in fork call\n");
		return -1;
	}

	/*child process execs display program*/
	if (pid1 == 0)
	{
		printf("Hello from display child!\n");
		rc = execlp("display", "display", pic, NULL);
		
		//shouldn't be reached
		printf("Execlp failure with exit code: %d", rc);
		return -1;
	}

	printf("hello from display parent, child process is: %d\n", pid1);

	return pid1;
}

int display(char *buffer1)
{
	printf("which image would you like to display: \n");
	input_string(">>>", buffer1, STRING_LEN);
	return execute_display(buffer1);
}

/**
 * @brief creates child process to create image thumnail
 * 
 * @param src source .jpg of large image
 * @param dest destination .jpg of thumbnail
 * @return int --  process id of child process, -1 on error  
 */
int execute_thumbnail(char *src, char *dest)
{
	int pid1 = 0;
	int rc = 0;

	pid1 = fork();

	if (pid1 < 0)
	{
		printf("error in fork call\n");
		return -1;
	}

	/*child process execs thumbnail program*/
	if (pid1 == 0)
	{
		printf("Hello from thumbnail child!\n");
		rc = execlp("convert", "convert", "-resize", "10%", src, dest, NULL);

		//shouldn't be reached
		printf("Execlp failure with exit code: %d", rc);
		return -1;
	}

	printf("hello from thumbnail parent, child process is: %d\n", pid1);

	return pid1;
}

int thumbnail(char *buffer1, char *buffer2)
{
	printf("which image would you like to generate a thumbnail for: \n");
	input_string(">>>", buffer1, STRING_LEN);
	printf("where would you like the thumbnail stored (must end in .jpg): \n");
	input_string(">>>", buffer2, STRING_LEN);
	execute_thumbnail(buffer1, buffer2);
}



/**
 * @brief allocates buffer
 * 
 * @return char* -- pointer to buffer created
 */
char *init_buffer()
{
	char *buf;

	buf = (char *)malloc(STRING_LEN);

	if (NULL == buf) {
		fprintf(stderr,"malloc error\n");
		exit(-1);
	}

	return buf;
}

int execute_rotate(char *src, char *dir, char *dest)
{
	int pid1 = 0;
	int rc = 0;

	pid1 = fork();

	if (pid1 < 0)
	{
		printf("error in fork call\n");
		return -1;
	}

	/*child process execs thumbnail program*/
	if (pid1 == 0)
	{
		printf("Hello from rotate child!\n");
		rc = execlp("convert", "convert", "-rotate", dir, src, dest, NULL);

		//shouldn't be reached
		printf("Execlp failure with exit code: %d", rc);
		return -1;
	}

	printf("hello from rotate parent, child process is: %d\n", pid1);

	return pid1;
}

int rotate(char *buffer1, char *buffer2, char *buffer3)
{
	/*finish gethering user input*/
	printf("which photo would you like to rotate:\n");
	input_string(">>>", buffer1, STRING_LEN);
	printf("which direction would you like to rotate the image: [l/r]:\n");
	input_string(">>>", buffer2, STRING_LEN);
	printf("where would you like the rotated stored (must end in .jpg): \n");
	input_string(">>>", buffer3, STRING_LEN);

	/*convert l/r to angle*/
	if (strcmp(buffer2, "l") == 0)
		strncpy(buffer2, "-90", STRING_LEN);
	else
		strncpy(buffer2, "90", STRING_LEN);

	return execute_rotate(buffer1, buffer2, buffer3);
}



int main (int argc, char *argv[])
{
	bool done = false;

	char *buffer1 = init_buffer();
	char *buffer2 = init_buffer();
	char *buffer3 = init_buffer();

	/*main parent loop*/
	while (!done)
	{
		display_menu();
		input_string(">>>", buffer1, STRING_LEN);
  		
		switch (buffer1[0])
		{
			case 'd':
				display(buffer1);
				break;
			case 't':
				thumbnail(buffer1, buffer2);
				break;
			case 'r':
				rotate(buffer1, buffer2, buffer3);
				break;
			case 'q':
				printf("Leaving album program\n");
				done = true;
				break;
			default:
				printf("default triggered\n");
				break;
		}
	}

	free(buffer1);
	free(buffer2);
	free(buffer3);
	return 0;
}

  
