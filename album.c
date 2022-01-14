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
#define HTML_SIZE 300
#define MAX_PROCESSES 5


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

/**
 * @brief Prints 2d char array
 * 
 * @param arr 
 * @param size - number of strings
 * @param wordsize -length of strings
 */
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

/**
 * @brief records caption
 * 
 * @param dest 
 */
void record_caption(char *dest)
{
	printf("What would  you like to caption the displayed photo max 31 characters:\n");
	input_string(">>>", dest, CAPTION_SIZE);
}

/**
 * @brief records direction of rotation ("\0" if no rotate)
 * 
 * @param dir 
 * @param buff - helper
 */
void record_direction(char *dir, char *buff)
{
	printf("Would you like to rotate the displayed picture[y/n]\n");
	input_string(">>>", buff, STRING_LEN);

	if (buff[0] == 'y')
	{
		printf("Which direction would you like to rotate the picture[l/r]\n");
		input_string(">>>", buff, STRING_LEN);

		if (buff[0] == 'l')
		{
			strncpy(dir, "-90", DIR_LEN);
		}
		else
		{
			strncpy(dir, "90", DIR_LEN);
		}
	}
	else
	{
		strncpy(dir, "\0", DIR_LEN);
	}
}

/**
 * @brief writes the entire picture's data to a file in html format
 * 
 * @param ofile - file being written to (should be index.html)
 * @param thumbnail 
 * @param medium -medium sized picture
 * @param caption 
 * @param buffer - helpful for debugging/building string
 */
void write_html(FILE *ofile, char *thumbnail, char *medium, char *caption, char *buffer)
{
	sprintf(buffer,
		"Please click on a thumbnail to view a medium-size image\n\n<h2>%s</h2>\n\n<a href=\"%s\"><img src=\"%s\" border=\"1\"></a>\n\n</body>\n\n\n\n",
		caption,
		medium,
		thumbnail);

	printf("buffer: %s\n", buffer);
	fprintf(ofile, buffer, HTML_SIZE);
}


/**
 * @brief prompt and record user for a rotation of the displayed picture and rotate if necessary
 * 
 * @param thumbnail - picture to be rotated
 * @param buffer1 - helpful for UI
 * @param buffer2 - helpful for UI
 * @param dir -for recording the direction
 * @return int 
 */
// int rotate_thumbnail(char *thumbnail, char *buffer1, char dir[4])
// {
// 	printf("Would you like to rotate the displayed picture[y/n]\n");
// 	input_string(">>>", buffer1, STRING_LEN);

// 	if (buffer1[0] == 'y')
// 	{
// 		printf("Which direction would you like to rotate the picture[l/r]\n");
// 		input_string(">>>", buffer1, STRING_LEN);

// 		if (buffer1[0] == 'l')
// 		{
// 			strncpy(dir, "-90", DIR_LEN);
// 		}
// 		else
// 		{
// 			strncpy(dir, "90", DIR_LEN);
// 		}
// 		return execute_rotate(thumbnail, thumbnail, dir);
// 	}
// 	else
// 	{
// 		strncpy(dir, "\0", DIR_LEN);
// 		return -1;
// 	}
// }


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

/**
 * @brief rotates a picture
 * 
 * @param src 
 * @param dest 
 * @param dir 
 * @return int -- process id of child process, -1 on error  
 */
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


void run_background_child(char *src, char *dir, char *thumbnail, char *medium_pic, int i, char *buffer1)
{
	int med_resize_pid, med_rotate_pid;
	int status, rc;
	int rotate_thumb_pid = execute_rotate(thumbnail, thumbnail, dir);

	printf("Hello from child\n");
	printf("	src: %s\n", src);
	printf("	dir: %s\n", dir);
	printf("	thumb: %s\n", thumbnail);
	printf("	med: %s\n", medium_pic);
	printf("	i: %d\n", i);

	strncpy(buffer1, "25%", STRING_LEN);
	med_resize_pid = execute_resize(src, medium_pic, buffer1);
	
	rc = waitpid(med_resize_pid, &status, 0);

	//rotate the medium
	med_rotate_pid = execute_rotate(medium_pic, medium_pic, dir);

	rc = waitpid(rotate_thumb_pid, &status, 0);
	rc = waitpid(med_rotate_pid, &status, 0);

	exit(0);
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
	char *buffer_large = (char *) malloc(sizeof(char) * HTML_SIZE);

	/*for tracking ongoing processes*/
	int thumbnail_pid_arr[argc - 1];
	int med_pid_arr[argc - 1];
	int display_pid_arr[argc - 1];
	int rotate_thumnail_pid_arr[argc - 1];
	int rotate_med_pid_arr[argc - 1];
	int background_pid_arr[argc - 1];
	int curr_pid = 0;
	int display_pid;
	
	/*for recording names of pictures/captions/rotates*/
	char thumbnail_names[argc - 1][THUMBNAIL_NAME_SIZE];
	char med_names[argc - 1][MED_NAME_SIZE];
	char caption_arr[argc - 1][CAPTION_SIZE];
	char rotate_arr[argc - 1][DIR_LEN];

	/*other*/
	int rc;
	int status;
	bool done = false;
	FILE *index = fopen("./index.html", "w");
	int thumbnail_index = 0;
	int medium_start_index = 0;
	int medium_rotate_index = 0;
	int i;
	
	strncpy(buffer_large, "<html><title>your album.html</title>\n<h1>your album.html</h1>\n", HTML_SIZE);
	fprintf(index, buffer_large, HTML_SIZE);
	
	strncpy(buffer3, "10%", STRING_LEN);

	/*loop through until all photos are converting to thumbnails or MAX_PROCESSes is hit*/
	for (i = 0; i < MAX_PROCESSES; i++)
	{
		/*if less photos than allowable concurrent processes stop when all photos done*/
		if (i == argc - 1)
		{
			break;
		}
		/*create thumbnail name and make thumbnail*/
		sprintf(thumbnail_names[i], "thumbnail_%d.jpg", i);
		thumbnail_pid_arr[i] = execute_resize(argv[i + 1], thumbnail_names[i], buffer3);
	}

	/*if not done making thumbnails, this is where to start back up*/
	thumbnail_index = i;

	/*loop through all pictures once their own thumbnail is done (not neccisarily others)*/
	for (i = 0; i < argc - 1; i++)
	{
		/*start once the first thumbnail is done being made*/
		rc = waitpid(thumbnail_pid_arr[i], &status, 0);

		/*if there are more thumbnails to make, make them*/
		if (thumbnail_index < argc - 1)
		{
			sprintf(thumbnail_names[thumbnail_index], "thumbnail_%d.jpg", thumbnail_index);
			thumbnail_pid_arr[thumbnail_index] = execute_resize(argv[thumbnail_index + 1], thumbnail_names[thumbnail_index], buffer3);
		}

		/*display and get user input on the photo*/
		display_pid = execute_display(thumbnail_names[i]);
		record_caption(caption_arr[i]);
		record_direction(rotate_arr[i], buffer1);
		sprintf(med_names[i], "med_%d.jpg", i);
		
		curr_pid = fork();

		if (curr_pid < 0)
		{
			printf("error in fork call\n");
			return -1;
		}

		if (curr_pid == 0)
		{
			//child do a bunch of stuff and then exit
			run_background_child(argv[i + 1], rotate_arr[i], thumbnail_names[i], med_names[i], i, buffer2);
		}
		else
		{
			//parent
			background_pid_arr[i] = curr_pid;
		}

		kill(display_pid, SIGTERM);
		thumbnail_index++;
	}
	for (int i = 0; i < argc - 1; i++)
	{
		rc = waitpid(background_pid_arr[i], &status, 0);
		
		write_html(index, thumbnail_names[i], med_names[i], caption_arr[i], buffer_large);
	}

	//rotate_thumnail_pid_arr[i] = rotate_thumbnail(thumbnail_names[i], buffer1, rotate_arr[i]);

	// i = 0;
	// while (medium_start_index < argc - 1)
	// {
	// 	rc = waitpid(med_pid_arr[i], &status, 0);

	// 	sprintf(med_names[medium_start_index], "med_%d.jpg", medium_start_index);
	// 	strncpy(buffer3, "25%", STRING_LEN);
	// 	med_pid_arr[medium_start_index] = execute_resize(argv[medium_start_index + 1], med_names[medium_start_index], buffer3);
	// 	medium_start_index++;

	// 	i++;
	// }

	// int j = 0;
	// while (i < argc - 1)
	// {
	// 	rc = waitpid(med_pid_arr[i], &status, 0);

	// 	if (rotate_thumnail_pid_arr[j] < 0)
	// 	{
	// 		rotate_med_pid_arr[j] = -1;
	// 		continue;
	// 	}

	// 	rotate_med_pid_arr[j] = execute_rotate(med_names[j], med_names[j], rotate_arr[j]);		

	// 	i++;
	// 	j++;
	// }
	
	//make sure i close rotate thumbnail pids

	// while (!done)
	// {
	// 	//for loop to start thumbnails
	// 	strncpy(buffer3, "10%", STRING_LEN);
		// for (int i = 0; i < MAX_PROCESSES; i++)
		// {
		// 	total_processed += 1;

		// 	if (total_processed == argc - 1)
		// 	{
		// 		done = true;
		// 	}

		// 	if (offset + i > argc - 1)
		// 	{
		// 		break;
		// 	}

		// 	sprintf(thumbnail_names[offset + i], "thumbnail_%d.jpg", offset + i);
		
		// 	thumbnail_pid_arr[offset + i] = execute_resize(argv[offset + i + 1], thumbnail_names[offset + i], buffer3);
		// }

	// 	printf("thumbnail name arr: \n");
	// 	print_string_arr((char **) thumbnail_names, offset + MAX_PROCESSES, THUMBNAIL_NAME_SIZE);

	// 	//start second for loop -- still need to check if i/total process is right
		// for (int i = 0; i < MAX_PROCESSES; i++)
		// {
		// 	if (offset + i > argc - 1)
		// 	{
		// 		break;
		// 	}

		// 	rc = waitpid(thumbnail_pid_arr[total_processed - 1], &status, 0);

		// 	display_pid = execute_display(thumbnail_names[i]);

		// 	record_caption(caption_arr[i]);

		// 	rotate_thumnail_pid_arr[i] = rotate_thumbnail(thumbnail_names[i], buffer1, rotate_arr[i]);

		// 	sprintf(med_names[i], "med_%d.jpg", i);
		// 	strncpy(buffer3, "25%", STRING_LEN);
		// 	med_pid_arr[i] = execute_resize(argv[i + 1], med_names[i], buffer3);		

		// 	kill(display_pid, SIGTERM);
		// }

	// 	printf("rotate arr:\n");
	// 	print_string_arr((char **) rotate_arr, argc - 1, DIR_LEN);

	// 	printf("med names:\n");
	// 	print_string_arr((char **) med_names, argc - 1, MED_NAME_SIZE);

		// for (int i = 0; i < MAX_PROCESSES; i++)
		// {	
		// 	if (offset + i > argc - 1)
		// 	{
		// 		break;
		// 	}
		// 	rc = waitpid(med_pid_arr[offset + i], &status, 0);

		// 	if (rotate_thumnail_pid_arr[offset + i] < 0)
		// 	{
		// 		rotate_med_pid_arr[offset + i] = -1;
		// 		continue;
		// 	}

		// 	rotate_med_pid_arr[offset + i] = execute_rotate(med_names[offset + i], med_names[offset + i], rotate_arr[offset + i]);
		// }

	// 	for (int i = 0; i < MAX_PROCESSES; i++)
	// 	{
	// 		if (offset + i > argc - 1)
	// 		{
	// 			break;
	// 		}
	// 		if (rotate_med_pid_arr[i] > 0)
	// 		{
	// 			rc = waitpid(rotate_med_pid_arr[i], &status, 0);
	// 		}

	// 		//could fork and make child do this
	// 		write_html(index, thumbnail_names[i], med_names[i], caption_arr[i], buffer_large);
	// 	}
	// 	offset += MAX_PROCESSES;
	// }

	

	printf("caption arr:\n");
	print_string_arr((char **) caption_arr, argc - 1, CAPTION_SIZE);

	printf("rotate arr:\n");
	print_string_arr((char **) rotate_arr, argc - 1, DIR_LEN);


	fclose(index);
	free(buffer1);
	free(buffer2);
	free(buffer3);
	free(buffer_large);
	return 0;
}

  
