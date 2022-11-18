#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Client.h"




int Client::init(uint16_t port, char* address)
{
	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		return SOCKET_ERROR;
	}

	sockaddr_in adr;

	adr.sin_port = htons(port);
	adr.sin_addr.s_addr = inet_addr(address);
	adr.sin_family = AF_INET;

	int check = connect(Socket, (SOCKADDR*)&adr, port);

	if (check == INVALID_SOCKET)
	{
		if (check == 0)
		{
			return SHUTDOWN;
		}
		else
		{
			return CONNECT_ERROR;
		}
	}

	if (Socket == INVALID_SOCKET)
	{
		return SOCKET_ERROR;
	}

	return SUCCESS;
}
int Client::readMessage(char* buffer, int32_t size)
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
int Client::sendMessage(char* data, int32_t length)
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
void Client::stop()
{
	shutdown(Socket, SD_BOTH);
	closesocket(Socket);
}