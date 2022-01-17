/**
 * @file process.h
 * @author Bobby Carangelo
 * @brief Helper functions to album.c that creates imagemagick processes to handle picture edits
 * @version 0.1
 * @date 2022-01-17
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once

#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <signal.h>   // for kill call
#include <unistd.h>   // for fork call
#include <sys/wait.h> // for waitpid


/**
 * @brief creates child process that runs image magick display program
 * 
 * @param pic name of picture to be displayed -- must end in .jpg (doesn't check for this)
 * @return int -- process id of child process, -1 on error  
 */
int execute_display(char *pic);

/**
 * @brief creates child process to create image thumnail
 * 
 * @param src source .jpg of large image
 * @param dest destination .jpg of thumbnail
 * @return int --  process id of child process, -1 on error  
 */
int execute_resize(char *src, char *dest, char *size);

/**
 * @brief rotates a picture
 * 
 * @param src 
 * @param dest 
 * @param dir 
 * @return int -- process id of child process, -1 on error  
 */
int execute_rotate(char *src, char *dest, char *dir);