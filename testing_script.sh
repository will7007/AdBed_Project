#!/bin/bash
#TODO@server.cpp:123: break out of this loop to rejoin main when a running variable inside of caller is set to false
#TODO: separate server class into successive child classes for each part for better organization (?)
files=("/home/will/Pictures/1humu.jpg" "/home/will/Pictures/2parrot.jpg" "/home/will/Pictures/3flamingos.jpg" "/home/will/Pictures/4Giraffes.jpg" "/home/will/Pictures/5below.jpg" "/home/will/Pictures/6flags.jpg" "/home/will/Pictures/7movie.jpg")

echo "Welcome to William's Advanced Embedded Sockets & Theads project!"
echo "Pick a number to test a different part of the project:"
echo "1 = single-threaded server"
echo "2 = multi-threaded server"
echo "3 = pre-threaded server"
echo "4 = pre-threaded server with priority"
read choice
echo "Now pick how many clients you would like to create (positive integer please)"
read clients
echo "Would you like the clients to display the image they send/receive from the server? (Y/n)"
read show_image

./server_program $choice &
serverPID=$!
if [[ $show_image == n* || $show_image == N* ]]
then
   echo "Not showing the images that the client sends/receives"
   for (( i = 0; i < $clients; i++ )) 
   do 
      ./client_program ${files[$((i % 7))]} x &
   done
else
   echo "Showing the images that the client sends/receives: press any key to close the images when they appear"
   for (( i = 0; i < $clients; i++ )) 
   do 
      ./client_program ${files[$((i % 7))]} &
   done
fi
read
echo "All done, closing server"
kill $serverPID