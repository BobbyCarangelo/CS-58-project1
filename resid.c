// /*main parent loop*/
	// while (!done)
	// {
	// 	display_menu();
	// 	input_string(">>>", buffer1, STRING_LEN);
  		
	// 	switch (buffer1[0])
	// 	{
	// 		case 'd':
	// 			display(buffer1);
	// 			break;
	// 		case 't':
	// 			thumbnail(buffer1, buffer2);
	// 			break;
	// 		case 'r':
	// 			rotate(buffer1, buffer2, buffer3);
	// 			break;
	// 		case 'c':
	// 			caption(buffer1, buffer2);
	// 			break;
	// 		case 'q':
	// 			printf("Leaving album program\n");
	// 			done = true;
	// 			break;
	// 		default:
	// 			printf("default triggered\n");
	// 			break;
	// 	}
	// }

    // int rotate(char *buffer1, char *buffer2, char *buffer3)
// {
// 	/*finish gethering user input*/
// 	printf("which photo would you like to rotate:\n");
// 	input_string(">>>", buffer1, STRING_LEN);
// 	printf("which direction would you like to rotate the image: [l/r]:\n");
// 	input_string(">>>", buffer2, STRING_LEN);
// 	printf("where would you like the rotated stored (must end in .jpg): \n");
// 	input_string(">>>", buffer3, STRING_LEN);

// 	/*convert l/r to angle*/
// 	if (strcmp(buffer2, "l") == 0)
// 		strncpy(buffer2, "-90", STRING_LEN);
// 	else
// 		strncpy(buffer2, "90", STRING_LEN);

// 	return execute_rotate(buffer1, buffer2, buffer3);
// }

// int caption(char *buffer1, char *buffer2)
// {
// 	printf("which image would you like to caption: \n");
// 	input_string(">>>", buffer1, STRING_LEN);
// 	return execute_caption(buffer1, buffer2);
// }

/**
 * @brief Display user options
 * 
 */
// void display_menu()
// {
// 	printf("Options:\n");
// 	printf("  'd' to display an image\n");
// 	printf("  't' to generate a thumbnail\n");
// 	printf("  'r' to rotate an image\n");
// 	printf("  'c' to caption an image\n");
// 	printf("  'q' to quit\n");
// }

// int display(char *buffer1)
// {
// 	printf("which image would you like to display: \n");
// 	input_string(">>>", buffer1, STRING_LEN);
// 	return execute_display(buffer1);
// }

// int thumbnail(char *buffer1, char *buffer2)
// {
// 	printf("which image would you like to generate a thumbnail for: \n");
// 	input_string(">>>", buffer1, STRING_LEN);
// 	printf("where would you like the thumbnail stored (must end in .jpg): \n");
// 	input_string(">>>", buffer2, STRING_LEN);
// 	execute_resize(buffer1, buffer2, "10%");
// }

/**
 * @brief 
 * 
 * @param pic 
 * @param caption 
 * @return int 
 */
// int execute_caption(char *pic, char *caption)
// {
// 	int pid1 = 0;
// 	int rc = 0;
// 	int status;

// 	pid1 = fork();

// 	if (pid1 < 0)
// 	{
// 		printf("error in fork call\n");
// 		return -1;
// 	}

// 	/*child process does caption program*/
// 	if (pid1 == 0)
// 	{
// 		printf("what would you like to make the caption for %s:\n", pic);
// 		input_string(">>>", caption, STRING_LEN);
// 		printf("caption entered: %s\n", caption);
// 		exit(0);
// 	}

// 	if (rc < 0)
// 	{
// 		printf("error waiting\n");
// 		return -1;
// 	}

// 	return pid1;
// }