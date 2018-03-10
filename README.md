# ConsoleChat

Client/Server messaging application written in C by Leonardo Lai, as final project for the exam "Computer networks".  
This is a terminal-based (no GUI) app to exchange text messages between clients, exploiting TCP/UDP sockets.  
The server is used to store pending messages when the clients are offline.  
Specification (in italian) are available here: http://www2.ing.unipi.it/c.vallati/files/reti/Progetto2017.pdf

**Files and folders hierarchy:**  
build -> contains most of the object files generated during compilation  
include -> contains small homemade libraries and side files  
Makefile -> just a makefile  
msg_client -> executable file (client)  
msg_server -> executable file (server)  
README -> this file  
src -> contains the most important source files, here's the core of the application  

More details (if available) can be found in the specific directories.
