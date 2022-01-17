/**
 * @file album_utils.h
 * @author Bobby Carangelo
 * @brief Helper functions to the album.c program
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

#include <unistd.h>   // for fork call


#define HTML_SIZE 300
#define STRING_LEN  32

// prompt the user with message, and save input at buffer
// (which should have space for at least len bytes)
int input_string(char *message, char *buffer, int len);

/**
 * @brief Prints 2d char array
 * 
 * @param arr 
 * @param size - number of strings
 * @param wordsize -length of strings
 */
void print_string_arr(char **arr, int size, int wordsize);

/**
 * @brief allocates buffer
 * 
 * @return char* -- pointer to buffer created
 */
char *init_buffer(int size);

/**
 * @brief writes the entire picture's data to a file in html format
 * 
 * @param ofile - file being written to (should be index.html)
 * @param thumbnail 
 * @param medium -medium sized picture
 * @param caption 
 * @param buffer - helpful for debugging/building string
 */
void write_html(FILE *ofile, char *thumbnail, char *medium, char *caption, char *buffer);