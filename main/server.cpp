#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <string>
#include <map>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 65432
#define BUFFER_SIZE 1024

std::vector<SOCKET> clients;
std::map<SOCKET, std::string> clientNicknames;
std::mutex clients_mutex;

void broadcastMessage(const std::string& message, SOCKET senderSocket) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (SOCKET clientSocket : clients) {
        if (clientSocket != senderSocket) {
            send(clientSocket, message.c_str(), message.length(), 0);
        }
    }
}

void handleClient(SOCKET clientSocket) {
    std::string nickname = "Anonimo";
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.push_back(clientSocket);
        clientNicknames[clientSocket] = nickname;
    }

    char buffer[BUFFER_SIZE];
    int bytesReceived;

    while (true) {
        ZeroMemory(buffer, BUFFER_SIZE);
        bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Errore nella ricezione dei dati: " << WSAGetLastError() << std::endl;
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "Il client ha chiuso la connessione.\n";
            break;
        }

        std::string message(buffer, bytesReceived);
        std::cout << "Messaggio ricevuto dal client: " << message << std::endl;

        if (message == "/quit") {
            std::cout << "Il client ha richiesto la disconnessione.\n";
            break;
        } else if (message.substr(0, 6) == "/nick ") {
            std::string newNickname = message.substr(6);
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clientNicknames[clientSocket] = newNickname;
            }
            std::string nickMessage = nickname + " ha cambiato nickname in " + newNickname + "\n";
            broadcastMessage(nickMessage, clientSocket);
            nickname = newNickname;
        } else if (message.substr(0, 4) == "/cl") {
            std::string commandsList = "Comandi disponibili:\n"
                                "/cl - Mostra la lista di tutti i comandi.\n"
                                "/nick <nuovo_nome> - Cambia il tuo nickname.\n"
                                "/quit - Disconnessione dal server.\n";
            send(clientSocket, commandsList.c_str(), commandsList.length(), 0);
        } else {
            std::string formattedMessage = "[" + nickname + "] " + message;
            broadcastMessage(formattedMessage, clientSocket);
        }
    }

    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        clients.erase(std::remove(clients.begin(), clients.end(), clientSocket), clients.end());
    }

    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        std::cerr << "Errore WSAStartup: " << result << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Errore nella creazione del socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Errore nel bind: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Errore nella listen: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::vector<std::thread> clientThreads;

    std::cout << "Server in ascolto sulla porta " << PORT << "...\n";

    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Errore nell'accept: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::cout << "Client connesso.\n";

        clientThreads.emplace_back(std::thread(handleClient, clientSocket));
    }

    for (auto& t : clientThreads) {
        if (t.joinable()) {
            t.join();
        }
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
