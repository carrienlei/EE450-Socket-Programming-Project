# EE450-Socket-Programming-Project
Carrie Lei

Student ID: 9718619209

## Overview + Completed Sections: 

For my EE450 Socket Programming project, I have created a library server system where clients with authenticated credentials can enter book queries and access their respective availability statuses. I have completed Phases 1-4 of the project criteria. This consists of: Boot-Up, Login and Confirmation, Frowarding Request to Backend Servers, and the Reply.

## Code Files and Purposes:

In my project, I have 5 CPP files, a Makefile, and this ReadMe.md file. 

For serverM.cpp, this is the main server that is boot-up first. It handles TCP communications with the client and UDP communications with the backend servers. It will handle the TCP book query from the client and send the request to the respective backend server via UDP.

For serverS.cpp, this is the science server that is boot-up second. It is in charge of keeping track of the science books and their availabilities. It communicates via UDP with the main server. 

For serverL.cpp, this is the literature server that is boot-up third. It is in charge of keeping track of the literature books and their availabilities. It communicates via UDP with the main server. 

For serverH.cpp, this is the history server that is boot-up third. It is in charge of keeping track of the history books and their availabilities. It communicates via UDP with the main server. 

For client.cpp, this is the client server that is boot-up last. It communicates via TCP with the main server and is where users can enter the book query they wish to check.

Lastly, there is a Makefile that compiles all the code into executables.

## Format of Exchanged Messages:

For serverM.cpp, the following messages are exchanged:
- [ "Main Server received the book code list from " << serverName << " using UDP over port " << port ] - This is printed when the main server receives the book code lists upon boot-up from the backend servers.
- [ "Main Server received the username and password from the client using TCP over port 45209." ] - This is printed when the main server receives login credentials from the client server.
- [ encryptedUsername << " is not registered. Send a reply to client" ] - This is printed when the login username is not found in member.txt.
- [ "Password " << encryptedPassword<< " does not match the username. Send a reply to the client" ] - This is printed when the username is found, but the password doesn't match.
- [ "Password " << encryptedPassword<< " matches the username. Send a reply to the client" ] - This is printed when the username and password are found and match.
- [ "Main Server received the book request from client using TCP over port 45209." ] - This is printed when the main server receives a book query request from the client.
- [ "Found " << bookCodeQuery << " located at Server " << backendServer<<". Send to Server "<< backendServer << "." ] - This is printed when the book query is found in the database, and serverM sends the request to the appropriate backend server.
- [ "Main Server received from server " << backendServer<< " the book status result using UDP over port " <<backendServerPort<<":\nNumber of books " << bookCodeQuery<< " available is: " << receivedMessage.substr(17) ] - This is printed after the main server receives a response from the backend server and denotes the number of books available left.
- ["Did not find " << bookCodeQuery << " in the book code list." ] - This is printed if the main server receives a book query that was not found in its database.
- [ "Main Server sent the book status to the client." ] - This is printed after the main server sends the book query's availability status back to the client.

For the backend servers (serverS.cpp, serverL.cpp, and serverH.cpp), the following messages are exchanged:
- [ "Book code found." ] - This is printed if the book code was found in the backend server _and_ the number of books left for that code is greater than 0.
- [ "Zero left in ServerS's database. Sending reply to ServerM" ] - This is printed if the book code was found in the backend server _and_ the number of books left for that code is 0.

For client.cpp, the following messages are exchanged:


