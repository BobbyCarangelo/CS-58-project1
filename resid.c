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