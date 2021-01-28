# ECGR 5101 Advanced Embedded Systems Project
This is my project for my Advanced Embedded Systems course, which I took at UNC Charlotte during the fall of 2020. I completed all four parts of this masters-level project in C/C++ shortly after the course finished as I was too busy with other classes to finish it while the course was underway. The full project description can be found under Project_handout.pdf, but here's a short summary of what I did in each part:
  
Part 1: Create a client and single-threaded server with sockets in Linux. Use OpenCV to transform an image received over the network into a modified version of itself (the operation in the prompt was grayscale, but I added in other transforms) and send the result back to the client, which will display the result. (server.cpp)

Part 2: Add multi-threading to the server, so that a new thread is spawned when a connection is accepted by the server. (serverThreaded.cpp)

Part 3: Pre-allocate the worker threads in the server and add a producer-consumer-style queue to the main thread. Accepted connection file descriptors will be placed in the queue and consumed by the worker threads which will work on the images and send them back. (serverPreThreaded.cpp)

Part 4: Adjust the priority of the main/worker threads so that the main thread has a higher priority than the worker threads. (serverPreThreaded.cpp)
