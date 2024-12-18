#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")

#define SERVER_IP "127.0.0.1"
#define PORT 65432
#define BUFFER_SIZE 1024

void receiveMessages(SOCKET clientSocket) {
    char buffer[BUFFER_SIZE];

    while (true) {
        ZeroMemory(buffer, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Errore durante la ricezione del messaggio.\n";
            break;
        } else if (bytesReceived == 0) {
            std::cout << "Il server ha chiuso la connessione.\n";
            break;
        }

        std::cout << "\n" << buffer << std::endl;
        std::cout << "> ";
        std::cout.flush();
    }
}

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "Errore WSAStartup: " << result << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Errore nella creazione del socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Errore nella connessione al server: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connessione al server stabilita.\n";

    std::string nickname = "Anonimo";
    std::thread receiveThread(receiveMessages, clientSocket);
    std::string message;

    while (true) {
        std::string input;
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input.empty()) {
            continue;
        }

        if (input == "exit") {
            std::cout << "Chiusura della connessione...\n";
            break;
        }

        if (input.substr(0, 6) == "/nick ") {
            nickname = input.substr(6);
            int bytesSent = send(clientSocket, input.c_str(), input.length(), 0);
            if (bytesSent == SOCKET_ERROR) {
                std::cerr << "Errore durante l'invio del messaggio: " << WSAGetLastError() << std::endl;
                break;
            }
            continue;
        }

        std::cout << "[" << nickname << "]: " << input << std::endl;
        
        int bytesSent = send(clientSocket, input.c_str(), input.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::cerr << "Errore durante l'invio del messaggio: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}

