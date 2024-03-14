#pragma once
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>

using namespace std;

bool initWinSock()
{
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (result != 0) {
		cout << "WSAStartup failed: " << result << '\n';
		return true;
	}
	return false;
}

sockaddr_in createSocket(int PORT)
{
	struct sockaddr_in socket;


	socket.sin_family = AF_INET;
	socket.sin_addr.s_addr = INADDR_ANY;
	socket.sin_port = htons(PORT);
	
	return socket;
}

SOCKET listen(sockaddr_in server, int PORT)
{
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {
		cout << "Error creating socket: " << WSAGetLastError() << '\n';
		WSACleanup();
	}


	if (bind(listenSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		cout << "Bind failed with error: " << WSAGetLastError() << '\n';
		closesocket(listenSocket);
		WSACleanup();
	}

	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed with error: " << WSAGetLastError() << '\n';
		closesocket(listenSocket);
		WSACleanup();
	}

	cout << "Server listening on port " << PORT << "...\n";

	return listenSocket;
}


void SendToClient(SOCKET listenSocket, char** arr)
{
	cout << "SEND";
	SOCKET acceptSocket = accept(listenSocket, nullptr, nullptr);
	char buffer[200];
	//printf("Enter the message: ");
	//cin.getline(buffer, 200);
	int sbyteCount = send(acceptSocket, buffer, 200, 0);

	cout << sbyteCount;
}
char* ReceiveFromClient(SOCKET listenSocket)
{
	SOCKET acceptSocket = accept(listenSocket, nullptr, nullptr);

	char receiveBuffer[200];
	int result = recv(acceptSocket, receiveBuffer, 200, 0);
	receiveBuffer[result] = '\0';  // Null-terminate the received data

	cout << receiveBuffer[0];
	return receiveBuffer;
}


void Communication(SOCKET* listenSocket)
{
	SOCKET clientSocket = accept(*listenSocket, nullptr, nullptr);
	if (clientSocket == INVALID_SOCKET) {
		cout << "Accept failed with error: " << WSAGetLastError() << '\n';
		closesocket(*listenSocket);
		WSACleanup();
		return;
	}

	cout << "Client connected.\n";

	// Handle client communication here

	closesocket(clientSocket);
}

SOCKET StartServer() {
	const int PORT = 4444;

	int init = initWinSock();

	if (init) return 0;

	struct sockaddr_in server = createSocket(PORT);

	SOCKET listenSocket = listen(server, PORT);
	return listenSocket;
}



void SendToServer(SOCKET clientSocket, char** arr)
{
	// Sending data to the server
	const char* sendData = "Hello, server!";
	send(clientSocket, sendData, strlen(sendData), 0);

}

char* ReceiveFromServer(SOCKET clientSocket)
{
	// Receiving data from the server
	char receiveBuffer[1024];
	int	result = recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0);


	receiveBuffer[result] = '\0';  // Null-terminate the received data
	return receiveBuffer;
}

SOCKET Client(const char* SERVER_IP)
{
	const int PORT = 4444;

	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		cout << "WSAStartup failed: " << result << '\n';
		return 1;
	}
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);

	result = connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR) {
		cout << "Error connecting to server: " << WSAGetLastError() << '\n';
		closesocket(clientSocket);
		WSACleanup();
		return 1;
	}

	cout << "Connected to server." << '\n';

	return clientSocket;
}