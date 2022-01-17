/**
 * @file album_utils.c
 * @author Bobby Carangelo
 * @brief Helper functions to the album.c program
 * @version 0.1
 * @date 2022-01-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include "album_utils.h"

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
char *init_buffer(int size)
{
	char *buf;

	buf = (char *)malloc(size);

	if (NULL == buf) {
		fprintf(stderr,"malloc error\n");
		exit(-1);
	}

	return buf;
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
	memset(buffer, '\0', HTML_SIZE);
	sprintf(buffer,
		"Please click on a thumbnail to view a medium-size image\n\n<h2>%s</h2>\n\n<a href=\"%s\"><img src=\"%s\" border=\"1\"></a>\n\n</body>\n\n\n\n",
		caption,
		medium,
		thumbnail);

	fprintf(ofile, buffer, HTML_SIZE);
}