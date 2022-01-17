# CS-58-project1
Operating Systems Project 1
Bobby Carangelo

## Overview

Album is a photo album editing program that allows users to rotate, and caption their pictures and writes the output to an html file. It uses mutliple processes and ImageMagick so that the user and the computer are never waiting on the other. The program is located in album.c and helper functions are contained in album_utils.h and process.h

## How to run

Compile the program by typing "make" in a terminal window, in the CS-58-project1 directory. Run the program by typing ./album <YOUR FIRST PICTURE.jpg> <YOUR SECOND PICTURE.jpg> ... <YOUR LAST PICTURE.jpg>. All pictures must be .jpg and must be located in the CS-58-project1 directory. The program will display a thumbnail for each picture, and will prompt the user for a caption and a rotation (y/n and l/r for yes or no and left or right) of the displayed photo (the display may take a couple of seconds to pop up). When done, the album will be save in a file called "index.html"

## How the concurrency works

 The program starts by converting a fixed amount of photos to thumbanils. As each one completes the following processes occurs:
    1) a new thumbanil conversion is started)
    2) the user is prompted for caption and rotation input
  
  Once the user inputs a caption and rotation for a photo, a child is forked to alter the thumbail and medium sized photos and the above is repeated until all user input is gathered
  
  As the processes that alter the pictures after the user input is recoreded finish, that photo's data is written to the html
  
  See the timeline for a visualization of the concurrent processes
