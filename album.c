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
#define THUMBNAIL_NAME_SIZE 20  //size of string: "thumbnail_0" ****will overflow if > 999,999 photos
#define MED_NAME_SIZE 13        //size of string: "med_0" ****will overflow if > 999,999 photos
#define CAPTION_SIZE 32
#define DIR_LEN 4               //size of string "-90"


/*************************** Utils ***************************/ 

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

/*************************** Process Creators ***************************/ 

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
		rc = execlp("display", "display", pic, NULL);
		
		//shouldn't be reached
		printf("Execlp failure with exit code: %d", rc);
		return -1;
	}

	return pid1;
}

/**
 * @brief creates child process to create image thumnail
 * 
 * @param src source .jpg of large image
 * @param dest destination .jpg of thumbnail
 * @return int --  process id of child process, -1 on error  
 */
int execute_resize(char *src, char *dest, char *size)
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
		rc = execlp("convert", "convert", "-resize", size, src, dest, NULL);

		//shouldn't be reached
		printf("Execlp failure with exit code: %d", rc);
		exit -1;
	}

	return pid1;
}

int execute_rotate(char *src, char *dest, char *dir)
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
		rc = execlp("convert", "convert", "-rotate", dir, src, dest, NULL);

		//shouldn't be reached
		printf("Execlp failure with exit code: %d", rc);
		return -1;
	}

	return pid1;
}

int execute_caption(char *pic, char *caption)
{
	int pid1 = 0;
	int rc = 0;
	int status;

	pid1 = fork();

	if (pid1 < 0)
	{
		printf("error in fork call\n");
		return -1;
	}

	/*child process does caption program*/
	if (pid1 == 0)
	{
		printf("what would you like to make the caption for %s:\n", pic);
		input_string(">>>", caption, STRING_LEN);
		printf("caption entered: %s\n", caption);
		exit(0);
	}

	if (rc < 0)
	{
		printf("error waiting\n");
		return -1;
	}

	return pid1;
}

/*************************** UI helpers ***************************/ 

int display(char *buffer1)
{
	printf("which image would you like to display: \n");
	input_string(">>>", buffer1, STRING_LEN);
	return execute_display(buffer1);
}

int thumbnail(char *buffer1, char *buffer2)
{
	printf("which image would you like to generate a thumbnail for: \n");
	input_string(">>>", buffer1, STRING_LEN);
	printf("where would you like the thumbnail stored (must end in .jpg): \n");
	input_string(">>>", buffer2, STRING_LEN);
	execute_resize(buffer1, buffer2, "10%");
}

int rotate_thumbnail(char *thumbnail, char *buffer1, char *buffer2, char dir[4])
{
	printf("Would you like to rotate the displayed picture[y/n]\n");
	input_string(">>>", buffer1, STRING_LEN);

	if (buffer1[0] == 'y')
	{
		printf("Which direction would you like to rotate the picture[l/r]\n");
		input_string(">>>", buffer1, STRING_LEN);

		if (buffer1[0] == 'l')
		{
			strncpy(dir, "-90", DIR_LEN);
		}
		else
		{
			strncpy(dir, "90", DIR_LEN);
		}
		return execute_rotate(thumbnail, thumbnail, dir);
	}
	else
	{
		strncpy(dir, "\0", DIR_LEN);
		return -1;
	}

}



int caption(char *buffer1, char *buffer2)
{
	printf("which image would you like to caption: \n");
	input_string(">>>", buffer1, STRING_LEN);
	return execute_caption(buffer1, buffer2);
}

void print_string_arr(char **arr, int size, int wordsize)
{
	char *buff = (char *) malloc(sizeof(char) * wordsize);
	for (int i = 0; i < size; i++)
	{
		strncpy(buff, (char *) arr + (i * wordsize), wordsize);
		printf("ind: %d - %s\n", i, buff);
	}
	free(buff);
}

void record_caption(char *dest)
{
	printf("What would  you like to caption the displayed photo max 31 characters:\n");
	input_string(">>>", dest, CAPTION_SIZE);
}


int main (int argc, char *argv[])
{
	//finish if not .jpgs specified
	if (argc <= 1)
	{
		printf("too few args\n");
		return 0;
	}

	char *buffer1 = init_buffer();
	char *buffer2 = init_buffer();
	char *buffer3 = init_buffer();

	/*for tracking ongoing processes*/
	int thumbnail_pid_arr[argc - 1];
	int med_pid_arr[argc - 1];
	int display_pid_arr[argc - 1];
	int rotate_thumnail_pid_arr[argc - 1];
	int rotate_med_pid_arr[argc - 1];
	int curr_pid = -1;
	int display_pid;
	
	/*for recording names of pictures/captions/rotates*/
	char thumbnail_names[argc - 1][THUMBNAIL_NAME_SIZE];
	char med_names[argc - 1][MED_NAME_SIZE];
	char caption_arr[argc - 1][CAPTION_SIZE];
	char rotate_arr[argc - 1][DIR_LEN];

	/*other*/
	int rc;
	int status;

	//for loop to start thumbnails
	strncpy(buffer3, "10%", STRING_LEN);
	for (int i = 1; i < argc; i++)
	{
		sprintf(thumbnail_names[i - 1], "thumbnail_%d.jpg", i);
	
		curr_pid = execute_resize(argv[i], thumbnail_names[i - 1], buffer3);

		thumbnail_pid_arr[i - 1] = curr_pid;
	}

	printf("thumbnail name arr: \n");
	print_string_arr((char **) thumbnail_names, argc - 1, THUMBNAIL_NAME_SIZE);

	//start second for loop
	for (int i = 0; i < argc - 1; i++)
	{
		rc = waitpid(thumbnail_pid_arr[i], &status, 0);

		display_pid = execute_display(thumbnail_names[i]);

		record_caption(caption_arr[i]);

		rotate_thumnail_pid_arr[i] = rotate_thumbnail(thumbnail_names[i], buffer1, buffer2, rotate_arr[i]);

		sprintf(med_names[i - 1], "med_%d.jpg", i);
		strncpy(buffer3, "25%", STRING_LEN);
		med_pid_arr[i] = execute_resize(argv[i + 1], med_names[i], buffer3);		

		kill(display_pid, SIGTERM);
	}

	printf("rotate arr:\n");
	print_string_arr((char **) rotate_arr, argc - 1, DIR_LEN);

	for (int i = 0; i < argc - 1; i++)
	{	
		rc = waitpid(med_pid_arr[i], &status, 0);

		if (rotate_thumnail_pid_arr[i] < 0)
		{
			continue;
		}

		rotate_med_pid_arr[i] = execute_rotate(med_names[i], med_names[i], rotate_arr[i]);
	}

	printf("caption arr:\n");
	print_string_arr((char **) caption_arr, argc - 1, CAPTION_SIZE);

	//loop through all and write to html

	free(buffer1);
	free(buffer2);
	free(buffer3);
	return 0;
}

  
