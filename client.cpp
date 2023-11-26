#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <map>
#include <cstring>

using namespace std;

// Encryption function - Used help from kindsOnTheGenius to understand string manipulation and Chat-GPT for structure inspiration as denoted in ReadMe
void encrypt(string& data) {
    for (char& a : data) {
        if (isalpha(a)) { // Handles characters
            char base = isupper(a) ? 'A' : 'a'; // Handles case-sensitivity
            a = ((a - base + 5) % 26) + base; // Shifts forward by 5
        } 
        else if (isdigit(a)) { // Handles digits
            a = ((a - '0' + 5) % 10) + '0';
        }
    }
}

void decrypt(string& data) { // opposite of encryption
    for (char& a : data) {
        if (isalpha(a)) {
            char base = isupper(a) ? 'A' : 'a';
            a = ((a - base + 21) % 26) + base;  
        } 
        else if (isdigit(a)) {
            a = ((a - '0' + 5) % 10) + '0'; 
        }
    }
}

int main() {
    cout << "Client is up and running." << endl;
    while (true) {

        // Set up the TCP client socket. Used help from Beej's Guide.
        int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == -1) {
            cerr << "Error creating client socket" << endl;
            return 1;
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); 
        serverAddress.sin_port = htons(45209); // ServerM's TCP port

        // Connect to the server
        if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
            cerr << "Error connecting to server" << endl;
            close(clientSocket);
            return 1;
        }

        // Prompt user login credentials.
        string username, password;
        cout << "Please enter the username: ";
        cin >> username;
        cout << "Please enter the password: ";
        cin >> password;

        encrypt(username);
        encrypt(password);
        string loginInfo = username + " " + password;

        // Send the encrypted login info to the server
        send(clientSocket, loginInfo.c_str(), loginInfo.size(), 0);

        decrypt(username); // decrypt username for printing purposes
        cout << username << " sent an authentication request to the Main Server." << endl;

        // Receive authentication info.
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived != -1) {
            buffer[bytesReceived] = '\0'; // Null-terminate the received data
            if (strcmp(buffer, "AuthenticationSuccessful") == 0) {
                cout << username << " received the result of authentication from Main Server using TCP over port 45209. Authentication is successful." << endl;

                while (true) { // Continue to ask for user queries.
                    string bookCode;
                    cout << "Please enter book code to query: ";
                    cin >> bookCode;

                    send(clientSocket, bookCode.c_str(), bookCode.size(), 0);
                    cout << username + " sent the request to the Main Server." << endl;

                    bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

                    if (bytesReceived != -1) {
                        buffer[bytesReceived] = '\0'; // Null-terminate the received data
                        cout << "Response received from the Main Server on TCP port: 45209." << endl;
                        if (strcmp(buffer, "BookAvailable") == 0) { // Used C++ strcmp help from source denoted in ReadMe
                            cout << "The requested book " << bookCode << " is available in the library.\n--- Start a new query ---" << endl;
                        } 
                        else if (strcmp(buffer, "BookNotAvailable") == 0) {
                            cout << "The requested book " << bookCode << " is NOT available in the library.\n--- Start a new query ---" << endl;
                        } 
                        else if (strcmp(buffer, "BookCodeNotFound") == 0) {
                            cout << "Not able to find the book code " << bookCode << " in the system.\n--- Start a new query ---" << endl;
                        } 
                        else {
                            cout << "Unexpected result from the server: " << buffer << endl;
                        }
                    } 
                    else {
                        cerr << "Error receiving result from the server" << endl;
                        break; 
                    }
                }

            } 
            else if (strcmp(buffer, "UsernameNotFound") == 0) {
                cout << username << "received the result of authentication from Main Server using TCP over port 45209. Authentication failed: Username is not found." << endl;
            } 
            else if (strcmp(buffer, "PasswordNotMatching") == 0) {
                cout << username << "received the result of authentication from Main Server using TCP over port 45209. Authentication failed: Password does not match." << endl;
            } 
            else {
                cout << "Unexpected result from the server: " << buffer << endl;
            }
        } 
        else {
            cerr << "Error receiving result from the server" << endl;
        }
        close(clientSocket);
    }
    return 0;
}
