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

#include "album_utils.h"
#include "process.h"

#define THUMBNAIL_NAME_SIZE 20  //size of string: "thumbnail_0" ****will overflow if > 999,999 photos
#define MED_NAME_SIZE 13        //size of string: "med_0" ****will overflow if > 999,999 photos
#define CAPTION_SIZE 32
#define DIR_LEN 4               //size of string "-90"

#define MAX_PROCESSES 2


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

void cleanup(FILE *ofile, char *buffer1, char *buffer2, char *buffer3, char *buffer_large)
{
	free(buffer1);
	free(buffer2);
	free(buffer3);
	free(buffer_large);
	fclose(ofile);
}

/**
 * @brief Finishes all the resizing and rotations of the thumbnail and medium picture for one photo in background, after user inputs info
 * 
 * @param src - original photo
 * @param dir -direction of rotation
 * @param thumbnail -thumbnail name
 * @param medium_pic -medium pic name
 * @param i -number of the photo
 * @param buffer1 -helper
 */
void run_background_child(char *src, char *dir, char *thumbnail, char *medium_pic, int i, char *buffer1, char *buffer2, char *buffer3, char *buffer_large, FILE *ofile)
{
	int med_resize_pid, med_rotate_pid, rotate_thumb_pid;
	int status, rc;

	strncpy(buffer1, "25%", STRING_LEN);
	med_resize_pid = execute_resize(src, medium_pic, buffer1);
	
	rc = waitpid(med_resize_pid, &status, 0);

	//rotate the medium and thumbnail
	if (strcmp(dir, "\0") != 0)
	{
		rotate_thumb_pid = execute_rotate(thumbnail, thumbnail, dir);
		med_rotate_pid = execute_rotate(medium_pic, medium_pic, dir);
	}

	rc = waitpid(rotate_thumb_pid, &status, 0);
	rc = waitpid(med_rotate_pid, &status, 0);

	//not sure if i need the line below
	//cleanup(ofile, buffer1, buffer2, buffer3, buffer_large);

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

	char *buffer1 = init_buffer(STRING_LEN);
	char *buffer2 = init_buffer(STRING_LEN);
	char *buffer3 = init_buffer(STRING_LEN  * 3);
	char *buffer_large = init_buffer(HTML_SIZE);

	/*for tracking ongoing processes*/
	int thumbnail_pid_arr[argc - 1];
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
	int thumbnail_index = 0;
	int i;
	
	/*start index file*/
	FILE *index = fopen("./index.html", "w");
	strncpy(buffer3, "<html><title>your album.html</title>\n<h1>your album.html</h1>\n\n", 65);
	fprintf(index, buffer3, 65);
	fflush(index);
	
	/*to start making thumbnails*/
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

		/*if there are more thumbnails to make, make them one at time*/
		if (thumbnail_index < argc - 1)
		{
			sprintf(thumbnail_names[thumbnail_index], "thumbnail_%d.jpg", thumbnail_index);
			thumbnail_pid_arr[thumbnail_index] = execute_resize(argv[thumbnail_index + 1], thumbnail_names[thumbnail_index], buffer3);
		}

		/*display and get user input on the photo*/
		display_pid = execute_display(thumbnail_names[i]);
		record_caption(caption_arr[i]);
		record_direction(rotate_arr[i], buffer1);
		
		/*create medium photo name*/
		sprintf(med_names[i], "med_%d.jpg", i);
		
		curr_pid = fork();

		if (curr_pid < 0)
		{
			printf("error in fork call\n");
			return -1;
		}

		if (curr_pid == 0)
		{
			//child finishes processing this photo in the background
			run_background_child(argv[i + 1], rotate_arr[i], thumbnail_names[i], med_names[i], i, buffer2, buffer1, buffer3, buffer_large, index);
		}
		else
		{
			//parent stores pid of child and continues iteration on next thumbail
			background_pid_arr[i] = curr_pid;
		}

		//kill current thumbnail display and move to next
		kill(display_pid, SIGTERM);
		thumbnail_index++;
	}

	/*loop through and write all data to index.html, waiting for the background processing to occur*/
	for (int i = 0; i < argc - 1; i++)
	{
		rc = waitpid(background_pid_arr[i], &status, 0);
		write_html(index, thumbnail_names[i], med_names[i], caption_arr[i], buffer_large);
	}

	//free memory
	cleanup(index, buffer1, buffer2, buffer3, buffer_large);

	return 0;
}

  
