# Simple Local IRC in C++ 

This project implements a basic IRC-like chat application in C++, designed to run locally on a single machine or a local network. It consists of a server and a client, both written in C++. The server handles multiple client connections and broadcasts messages to all connected clients, while the client allows users to connect to the server, send messages, and change their nickname.

## Features

- **Local chat functionality**: Users can send and receive messages in real-time within a local environment (same machine or local network).
- **Nickname support**: Clients can change their nickname using the `/nick <new_nickname>` command.
- **Broadcasting messages**: The server broadcasts each message to all connected clients.
- **Command list**: Users can view a list of available commands with the `/cl` command.
- **Exit command**: Users can disconnect from the server using the `/quit` command.

## How to Use

1. **Start the server**:
   - Compile and run the server code on the machine you want to act as the server.
   - The server listens for incoming connections on a specific port (`65432` by default) and is designed to run locally.

2. **Start the client**:
   - Compile and run the client code on the same machine or any machine within the local network.
   - The client will connect to the server using the local IP address (`127.0.0.1` for the same machine, or the server's local network IP for different machines on the same network).

3. **Interact**:
   - Send messages to other clients connected to the same server.
   - Change your nickname using the `/nick <new_nickname>` command.
   - View available commands with `/cl`.
   - Exit the chat by typing `/quit`.

## Commands

- **`/nick <new_nickname>`**: Change your nickname.
- **`/cl`**: Display a list of available commands.
- **`/quit`**: Disconnect from the server.

## Compilation and Requirements

### Prerequisites

- **C++ Compiler** (e.g., GCC, MSVC)
- **Winsock Library** (for Windows-based systems)

### Compilation

To compile the server and client:

1. Use your preferred C++ compiler.
2. Link with the `Ws2_32.lib` library to use Winsock functions.

For example, on a Windows machine using `g++`:

```bash
g++ server.cpp -o server -lws2_32
g++ client.cpp -o client -lws2_32
```

### Explanation of Changes:
- **"Local" specification**: I've added more emphasis on the fact that the server and client are meant to run in a local environment, either on the same machine or on different machines within the same local network.
- **Server and Client Setup**: Clarified that the server listens on `127.0.0.1` (localhost) by default, and clients must connect to this IP for local usage or to the server's local network IP for remote connections within the same network.

This updated README now clearly communicates that the application is intended for local use.
