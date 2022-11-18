#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"

int Server::init(uint16_t port)
{
	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
	{
		return SOCKET_ERROR;
	}

	sockaddr_in adr;
	adr.sin_port = htons(port);
	adr.sin_addr.S_un.S_addr = INADDR_ANY;
	adr.sin_family = AF_INET;

	int check = bind(ListenSocket, (SOCKADDR*)&adr, sizeof(adr));
	if (check == SOCKET_ERROR)
	{
		return BIND_ERROR;
	}

	check = listen(ListenSocket, SOMAXCONN);
	if (check == SOCKET_ERROR)
	{
		return SETUP_ERROR;
	}

	Socket = accept(ListenSocket, NULL, NULL);
	
	if (Socket == INVALID_SOCKET)
	{
		if (Socket == 0)
		{
			return SHUTDOWN;
		}
		if (Socket == SOCKET_ERROR)
		{
			return CONNECT_ERROR;
		}
		return SETUP_ERROR;
	}

	return SUCCESS;
}
int Server::readMessage(char* buffer, int32_t size)
{
	int result = recv(Socket, buffer, 1, 0);

	if (result == 0)
	{
		return SHUTDOWN;
	}

	if (result == SOCKET_ERROR)
	{
		return DISCONNECT;
	}

	size_t msgSize = (*buffer) & 0xFF;

	if (msgSize > size)
	{
		return PARAMETER_ERROR;
	}

	size_t bytesRecvd = 0;

	while (bytesRecvd < msgSize)
	{
		result = recv(Socket, buffer + bytesRecvd, msgSize - bytesRecvd, 0);
		if (result == 0)
		{
			return SHUTDOWN;
		}
		if (result == SOCKET_ERROR)
		{
			return DISCONNECT;
		}
		bytesRecvd += result;
	}
	return SUCCESS;
}
int Server::sendMessage(char* data, int32_t length)
{
	if (length < 0 || length > 255)
	{
		return PARAMETER_ERROR;
	}

	char sendSize = length;
	int result = send(Socket, &sendSize, 1, 0);
	if (result == 0)
	{
		return SHUTDOWN;
	}

	if (result == SOCKET_ERROR)
	{
		return DISCONNECT;
	}

	result = send(Socket, data, length, 0);

	if (result == 0)
	{
		return SHUTDOWN;
	}

	if (result == SOCKET_ERROR)
	{
		return DISCONNECT;
	}

	return SUCCESS;
}
void Server::stop()
{
	shutdown(Socket, SD_BOTH);
	closesocket(Socket);

	shutdown(ListenSocket, SD_BOTH);
	closesocket(ListenSocket);
}