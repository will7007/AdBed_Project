#!/bin/bash
#TODO: (optional, not part of the project) Still need to implement the ability for the client to shut down the server via a request
#telling the single-threaded server to shut down is easy--the issue arrises when the main thread waits on another connection before the thread can report that the server should shut down
files=("/home/will/Pictures/1humu.jpg" "/home/will/Pictures/2parrot.jpg" "/home/will/Pictures/3flamingos.jpg" "/home/will/Pictures/4Giraffes.jpg" "/home/will/Pictures/5below.jpg" "/home/will/Pictures/6flags.jpg" "/home/will/Pictures/7movie.jpg")

echo "Welcome to William's Advanced Embedded Sockets & Theads project!"
echo "Make sure you are running this script as a ROOT USER so we can make sockets."
echo "Pick a number to test a different part of the project:"
echo "1 = single-threaded server"
echo "2 = multi-threaded server"
echo "3 = pre-threaded server"
echo "4 = pre-threaded server with priority"
read choice
echo "Now pick how many clients you would like to create (positive integer please)"
read clients
echo "What operation would you like to be performed on the images?"
echo "0b001=grayscale, 0b010=y-axis flip, 0b100=50% resize"
echo "These operations can be combined in one number (for example, 5=0b101=grayscale and 50% resize"
read operations
echo "Would you like the clients to display the image they send/receive from the server? (Y/n)"
read show_image

./server_program $choice &
serverPID=$!
if [[ $show_image == n* || $show_image == N* ]]
then
   echo "Not showing the images that the client sends/receives"
   for (( i = 0; i < $clients; i++ )) 
   do 
      ./client_program ${files[$((i % 7))]} $operations x &
   done
else
   echo "Showing the images that the client sends/receives: press any key to close the images when they appear"
   for (( i = 0; i < $clients; i++ )) 
   do 
      ./client_program ${files[$((i % 7))]} $operations &
   done
fi

# if [ $choice -ge 3 ]
# then
   read
   echo "All done, killing server"
   echo $serverPID
   kill $serverPID
# else
#    read
#    echo "All done, client asking server to shut down"
#    ./client_program ${files[0]} 0
# fi