#Info
This is a simple terminal chat program made using pthreads and sockets, it is rough around the edges but showcases my use of C.

Users join a chatroom, specified by port, and communicate with each other through sockets.

#Instructions
Users who enter the chat room can type whatever they wish. A user wanting to leave the chat can type `!` by itself. Typing anything alongside `!`, such as `hello!`, does not terminate the program.

To compile the project, run
```
make
```
To remove the project, run
```
make clean
```

To initiate chat type
```
./s-talk [port #] [remote machine name] [remote port #]
```
Do this in two different terminals to see it in action locally

#Additional Info
Some things in this assignment were not implemented correctly but overall I learned alot and took away some valuable lessons from this. Uploading for showcasing purposes.
