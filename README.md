# EE450-Socket-Programming-Project
Carrie Lei

Student ID: 9718619209

## 
Overview + Completed Sections:

For my EE450 Socket Programming project, I have created a library server system where clients with authenticated credentials can enter book queries and access their respective availability statuses. I have completed Phases 1-4 of the project criteria. This consists of: Boot-Up, Login and Confirmation, Frowarding Request to Backend Servers, and the Reply.

##
Code Files and Purposes:

In my project, I have 5 CPP files, a Makefile, and this ReadMe.md file. 

For serverM.cpp, this is the main server that is boot-up first. It handles TCP communications with the client and UDP communications with the backend servers. It will handle the TCP book query from the client and send the request to the respective backend server via UDP.

For serverS.cpp, this is the science server that is boot-up second. It is in charge of keeping track of the science books and their availabilities. It communicates via UDP with the main server. 

For serverL.cpp, this is the literature server that is boot-up third. It is in charge of keeping track of the literature books and their availabilities. It communicates via UDP with the main server. 

For serverH.cpp, this is the history server that is boot-up third. It is in charge of keeping track of the history books and their availabilities. It communicates via UDP with the main server. 

For client.cpp, this is the client server that is boot-up last. It communicates via TCP with the main server and is where users can enter the book query they wish to check.

Lastly, there is a Makefile that compiles all the code into executables.

##
Format of Exchanged Messages:

For serverM.cpp, the following messages are exchanged:
- [ "Main Server received the book code list from " << serverName << " using UDP over port " << port ]

