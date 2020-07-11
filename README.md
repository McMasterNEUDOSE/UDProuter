# UDP Server Mutli Client Application in C

## Introduction
These applications were developed as proof-of-concept for functionality 
which will possibly be included in a satellite command and data handling 
system running NASA's Core Flight Executive (cFE).

## Purpose
The applications provide functionality for UDP multi-port processing. 
The router.c application acts as a server which uses the select() 
functionality to wait for readable input from multiple UDP sockets. A 
timeout is set in the server so that other work could be done in the 
loop if a packet had not been received within the time limit. The client 
application forks an additional process and both the parent and child 
processes create their own socket which addresses the server 
application. Each process has a unique port to send messages to the 
server application.

## Build
To build the applications in Linux using gcc, run `make` in the terminal. The files will be executable via `./router` and `./client`. The router application should be executed before the client application.
