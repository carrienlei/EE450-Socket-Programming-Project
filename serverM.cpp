#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>
#include <arpa/inet.h>
#include <map>

using namespace std;

// Function to read the backend server values and populate the main database.
void receiveAndPrintDatabase(int port, const char* serverName, map<string, int>& combinedDatabase) {
    
    // Create UDP socket. Code inspiration from Beej's guide and lecture notes.
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        cout << "Error creating UDP socket in serverM" << endl;
        return;
    }
    sockaddr_in backendServerAddress;
    backendServerAddress.sin_family = AF_INET;
    backendServerAddress.sin_addr.s_addr = INADDR_ANY;
    backendServerAddress.sin_port = htons(port);

    // Bind the socket to a specific address and port
    if (bind(udpSocket, (struct sockaddr*)&backendServerAddress, sizeof(backendServerAddress)) == -1) {
        cout<< "Error binding UDP socket in serverMain" << endl;
        close(udpSocket);
        return;
    }

    // Receive data from backend server. Code inspired by Beej's guide and Geek for Geeks article on UDP communication as denoted in ReadMe.
    char buffer[1024];
    sockaddr_in senderAddress;
    socklen_t senderAddressSize = sizeof(senderAddress);
    int bytesReceived = recvfrom(udpSocket, buffer, sizeof(buffer), 0,
                                 (struct sockaddr*)&senderAddress, &senderAddressSize);

    if (bytesReceived != -1) { // Data received from backend server.
        buffer[bytesReceived] = '\0'; 
        cout << "Main Server received the book code list from " << serverName << " using UDP over port " << port << endl;

        // Process and store the received data into the combined main database. Code inspired from Stack Overflow article on reading in strings and using getline as denoted in ReadMe.
        istringstream iss(buffer);
        string line;
        while (getline(iss, line)) { 
            istringstream lineStream(line);
            string bookCode;
            string amountStr;
            if (getline(lineStream, bookCode, ',') && getline(lineStream, amountStr)) {
                    try {
                        int amount = stoi(amountStr);
                        combinedDatabase[bookCode] = amount;
                    } 
                    catch (const invalid_argument& e) {
                        cout<< "Invalid amount: " << amountStr << endl;
                    }
                }
            }
        } 
        else { // Data not received from backend server.
            cout<< "Error receiving data" << endl;
        }
    close(udpSocket);
}

// Decryption function (opposite of encryption)
void decrypt(string& data) {
    for (char& x : data) {
        if (isalpha(x)) {
            char base = isupper(x) ? 'A' : 'a';
            x = ((x - base + 21) % 26) + base; 
        } else if (isdigit(x)) {
            x = ((x - '0' + 5) % 10) + '0'; 
        }
    }
}

// Function that handles all TCP communications between serverM and client.
void TCPwithClient(int port, map<string, string>& members, map<string, int> combinedDatabase) {
    
    // Set up the TCP server socket. Code inspiration from Beej's guide and prior EE250 programming Lab.
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cout<< "Error creating server socket" << endl;
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // Bind the socket to a specific address.
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cout<< "Error binding server socket" << endl;
        close(serverSocket);
        return;
    }

    while (true) { // Continues to listen for client queries.
        
        // Listen for incoming connections
        if (listen(serverSocket, 5) == -1) {
            cerr << "Error listening for connections" << endl;
            close(serverSocket);
            return;
        }

        // Accept incoming connection
        sockaddr_in clientAddress;
        socklen_t clientAddressSize = sizeof(clientAddress);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientAddressSize);

        if (clientSocket == -1) {
            cout<< "Error accepting" << endl;
            return;
        }

        // Receive username and password from the client
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived != -1) {
            cout << "Main Server received the username and password from the client using TCP over port 45209." << endl;
            buffer[bytesReceived] = '\0'; 
            string encryptedLoginInfo(buffer);
            istringstream loginInfoStream(encryptedLoginInfo);
            string encryptedUsername, encryptedPassword;
            if (loginInfoStream >> encryptedUsername >> encryptedPassword) {
                auto it = members.find(encryptedUsername);
                if (it == members.end()) { // Username not found
                    decrypt(encryptedUsername); // revert back to decrypted form for print statement
                    cout << encryptedUsername << " is not registered. Send a reply to client" << endl;
                    send(clientSocket, "UsernameNotFound", sizeof("UsernameNotFound"), 0);
                } 
                else { // Check if the password matches
                    if (it->second != encryptedPassword) { // Password does not match 
                        decrypt(encryptedPassword);
                        cout << "Password " << encryptedPassword<< " does not match the username. Send a reply to the client" << endl;
                        send(clientSocket, "PasswordNotMatching", sizeof("PasswordNotMatching"), 0);
                    } 
                    else { // Password matches
                        decrypt(encryptedPassword);
                        cout << "Password " << encryptedPassword<< " matches the username. Send a reply to the client" << endl;
                        send(clientSocket, "AuthenticationSuccessful", sizeof("AuthenticationSuccessful"), 0);

                        while (true) { // Continues to listen for client queries.
                            bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                            if (bytesReceived != -1) {
                                cout << "Main Server received the book request from client using TCP over port 45209." << endl;
                                buffer[bytesReceived] = '\0'; 
                                string bookCodeQuery(buffer);

                                // Check if the book code is in the combined database
                                auto bookIterator = combinedDatabase.find(bookCodeQuery);
                                if (bookIterator != combinedDatabase.end()) { // in database
                                    char backendServer = bookCodeQuery[0];
                                    int backendServerPort = 0;
                                    cout << "Found " << bookCodeQuery << " located at Server " << backendServer<<". Send to Server "<< backendServer << "." << endl;                                   

                                    switch (backendServer) { // switch statements to determine which backend server to go to
                                        case 'S':
                                            backendServerPort = 41209; 
                                            break;
                                        case 'L':
                                            backendServerPort = 42209; 
                                            break;
                                        case 'H':
                                            backendServerPort = 43209; 
                                            break;
                                        default:
                                            cout << "Error" << endl;
                                            break;
                                    }

                                    // Connect to backend server via UDP (same UDP socket programming as before).
                                    int backendServerSocket = socket(AF_INET, SOCK_STREAM, 0);
                                    if (backendServerSocket == -1) {
                                        cout<< "Error creating backend server socket" << endl;
                                        return;
                                    }

                                    sockaddr_in backendServerAddress;
                                    backendServerAddress.sin_family = AF_INET;
                                    backendServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Backend server's IP address
                                    backendServerAddress.sin_port = htons(backendServerPort);

                                    if (connect(backendServerSocket, (struct sockaddr*)&backendServerAddress, sizeof(backendServerAddress)) == -1) {
                                        cout<< "Error connecting to backend server" << endl;
                                        close(backendServerSocket);
                                        return;
                                    }

                                    // Send the book code to the backend server
                                    send(backendServerSocket, bookCodeQuery.c_str(), bookCodeQuery.size(), 0);

                                    // Receive the result from the backend server
                                    bytesReceived = recv(backendServerSocket, buffer, sizeof(buffer), 0);
                                    buffer[bytesReceived] = '\0';

                                    if (bytesReceived != -1) {
                                        string receivedMessage(buffer);
                                        istringstream messageStream(receivedMessage);
                                        string status;
                                        int amount;

                                        if (receivedMessage[0] == 'M') { // More than one book left.
                                            // cout << "Amount: " << receivedMessage.substr(17) << endl;
                                            cout << "Main Server received from server " << backendServer<< " the book status result using UDP over port " <<backendServerPort<<":\nNumber of books " << bookCodeQuery<< " available is: " << receivedMessage.substr(17) << endl;
                                            send(clientSocket, "BookAvailable", sizeof("BookAvailable"), 0);
                                            cout << "Main Server sent the book status to the client." << endl;
                                        } 
                                        else { // Zero of the book left.
                                            cout << "Main Server received from server " << backendServer<< " the book status result using UDP over port " <<backendServerPort<<":\nNumber of books " << bookCodeQuery<< " available is: 0" << endl;
                                            send(clientSocket, "BookNotAvailable", sizeof("BookNotAvailable"), 0);
                                            cout << "Main Server sent the book status to the client." << endl;   
                                        }
                                    } 
                                    close(backendServerSocket);
                                } 
                                else { // Book code not found in serverM combined database.
                                    cout << "Did not find " << bookCodeQuery << " in the book code list." << endl;
                                    send(clientSocket, "BookCodeNotFound", sizeof("BookCodeNotFound"), 0);
                                    cout << "Main Server sent the book status to the client." << endl;
                                }
                            } 
                            else {
                                cout<< "Error receiving book code" << endl;
                                break; 
                            }
                        }
                    }
                }
            } 
            else {
                cout<< "Error with login info" << endl;
            }
        } 
        else {
            cout<< "Error in receiving login info" << endl;
        }
        close(clientSocket);
    }
    close(serverSocket);
}

int main() {
    cout << "Main Server is up and running" << endl;

    map<string, int> combinedDatabase; // Store all initial books on startup.

    // Specify the UDP ports of backend servers.
    int serverSciencePort = 41209;
    int serverLiteraturePort = 42209;
    int serverHistoryPort = 43209;

    // Receive and print the respective backend databases.
    receiveAndPrintDatabase(serverSciencePort, "server S", combinedDatabase);
    receiveAndPrintDatabase(serverLiteraturePort, "server L", combinedDatabase);
    receiveAndPrintDatabase(serverHistoryPort, "server H", combinedDatabase);

    // Read Members.txt Used same code as before with inspiration from Stack Overflow post denoted in ReadMe.
    string data;
    ifstream myfile ("member.txt");
    map<string, string> memberDatabase;
    while (getline (myfile, data)) {
        stringstream ss(data);
        string login;
        string password;
        if (getline(ss, login, ',') && ss >> password) {
            memberDatabase[login] = password;
        } else {
            cout<< "Error reading line " << data << endl;
        }
    }
    cout << "Main Server loaded the member list." << endl;
    
    // Handle TCP communications with client and UDP communications with respective backend servers when needed.
    TCPwithClient(45209, memberDatabase, combinedDatabase);
    return 0;
}