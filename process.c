/**
 * @file process.c
 * @author Bobby Carangelo
 * @brief Helper functions to album.c that creates imagemagick processes to handle picture edits
 * @version 0.1
 * @date 2022-01-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "process.h"



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
