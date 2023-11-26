#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <map>

using namespace std;

// Function that returns the backend server's database and sends info to main server.
map<string, int> readAndSendDatabase(const char* fileName, int serverPort) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cout << "Error opening file: " << endl;
    }

    string data;
    ifstream myfile (fileName);
    map<string, int> scienceDatabase;

    while (getline (myfile, data)) {
        stringstream ss(data);
        string bookCode;
        int amount;
        if (getline(ss, bookCode, ',') && ss >> amount) {
            scienceDatabase[bookCode] = amount;
        } 
        else {
            cout << "Error parsing line: " << data << endl;
        }
    }
    myfile.close();

    // Set up the UDP socket for sending database to serverM. Code guided by Beej's guide.
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSocket == -1) {
        cout << "Error creating UDP socket in serverScience" << endl;
    }

    sockaddr_in serverMainAddress;
    serverMainAddress.sin_family = AF_INET;
    serverMainAddress.sin_port = htons(41209); 
    inet_pton(AF_INET, "127.0.0.1", &serverMainAddress.sin_addr); 

    // turn database into string for sending to serverM. Code inspired from file on how to handle strings as denoted in ReadMe.
    ostringstream messageStream;
    for (const auto& entry : scienceDatabase) {
        messageStream << entry.first << "," << entry.second << "\n";
    }
    string message = messageStream.str();

    // Send the message to serverMain
    ssize_t sentBytes = sendto(udpSocket, message.c_str(), message.size(), 0,
                               (struct sockaddr*)&serverMainAddress, sizeof(serverMainAddress));
    close(udpSocket);
    return scienceDatabase;
}

// File for handling book queries from serverM.
void UDPConnection(int port, map<string, int> database) {

    // Code guided by Beej's guide.
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cout << "Error creating server socket" << endl;
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    // Bind the server
    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        cout << "Error binding server socket" << endl;
        close(serverSocket);
        return;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        cout << "Error listening for connections" << endl;
        close(serverSocket);
        return;
    }

    while (true) { // While loop to continuously listen for new requests.
        // Accept incoming connection
        sockaddr_in serverM;
        socklen_t serverMSize = sizeof(serverM);
        int serverMSocket = accept(serverSocket, (struct sockaddr*)&serverM, &serverMSize);

        if (serverMSocket == -1) {
            cout << "Error accepting connection" << endl;
            close(serverSocket);
            return;
        }

        // Receive the book code from the client
        char buffer[1024];
        int bytesReceived = recv(serverMSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived != -1) {
            buffer[bytesReceived] = '\0'; // Null-terminate the received data
            string bookCodeQuery(buffer);

            // Check if the book code is in the serverS database. Used explanation from Chat GPT to write the loop to search for bookcode and its respective amount.
            auto bookCodeIt = database.find(bookCodeQuery);
            if (bookCodeIt != database.end() && bookCodeIt->second > 0) { // Book code found and more than 0 left
                cout << "Book code found." << endl;

                ostringstream replyStream;
                replyStream << "MoreThanZeroLeft," << bookCodeIt->second;
                string replyMessage = replyStream.str();

                // Send result with book status and amount left to ServerM
                send(serverMSocket, replyMessage.c_str(), replyMessage.size(), 0);
                database[bookCodeQuery] --; // decrement the amount after

            } 
            else {
                cout << "Zero left in ServerS's database. Sending reply to ServerM" << endl;
                send(serverMSocket, "ZeroLeft", sizeof("ZeroLeft"), 0);
            }
        } 
        else {
            cout << "Error receiving book code from the client" << endl;
        }
        close(serverMSocket);
    }
    close(serverSocket);
}

int main() {
    map<string, int> database;
    database = readAndSendDatabase("science.txt", 41209);
    UDPConnection(41209, database);
    return 0;
}