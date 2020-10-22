// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#define PORT 8080
#define PACKET_SIZE 128
using namespace std;

int server_fd, new_socket, valread;
struct sockaddr_in clientAddress, serverAddress;
struct sockaddr_storage storage;
int opt = 1;
socklen_t cliLength = sizeof(clientAddress);
socklen_t addrSize;
char buffer[1024] = {0};
char *hello = "Hello from server";

int calculateChecksum(char packet[]);
int receiveMessage();
int connectToClient();

int main(int argc, char const *argv[])
{
	connectToClient();

	receiveMessage();

	// valread = read(new_socket, buffer, 1024);
	// cout << buffer << endl;
	send(new_socket, hello, strlen(hello), 0);
	cout << "Hello message sent" << endl;
	return 0;
}

int connectToClient()
{
	// Creating socket file descriptor
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}

	// Forcefully attaching socket to the port 8080
	// if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
	// {
	// 	perror("setsockopt");
	// 	exit(EXIT_FAILURE);
	// }
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(PORT);

	// Forcefully attaching socket to the port 8080
	if (::bind(server_fd, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(server_fd, 3) < 0)
	{
		perror("listen");
		exit(EXIT_FAILURE);
	}
	if ((new_socket = accept(server_fd, (struct sockaddr *)&serverAddress,
							 (socklen_t *)&addrSize)) < 0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}

	addrSize = sizeof(storage);
}

int calculateChecksum(char packet[])
{
	// Variable declarations
	int sum = 0;
	int i;

	// Calculates the sum
	for (i = 1; i < 10; i++)
	{
		sum += packet[i];
	}

	// Returns true (1) if checksum contains all 3's; otherwise, returns false (0)
	if (sum % 3 == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int receiveMessage()
{
	// Variable declarations
	char c;
	int packetNumber = 1;
	ofstream file;
	file.open("TestFile");

	// Checks if file opened properly
	if (!file.is_open())
	{
		printf("Can't open output file");
		exit(EXIT_FAILURE);
	}

	// int n = 240 * 1024;
	// setsockopt(server_fd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));

	while (c != '*')
	{
		int passed;
		if (recvfrom(server_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *)&clientAddress, &cliLength) == -1)
		{
			perror("Error receiving message!!!");
			exit(EXIT_FAILURE);
		}

		printf("\n\nReceived packet %d...\n", packetNumber);
		packetNumber++;
		printf("The packet is being checked for errors...\n");
		passed = calculateChecksum(buffer);
		if (passed == 1)
		{
			printf("The packet was valid!\n");
			printf("Message reads:\n%s(%lu bytes).", buffer, sizeof(buffer));
			int i = 0;
			// fflush(of);
			for (i = 11; i < PACKET_SIZE; i++)
			{
				file << buffer[i];
				// putc(buffer[i], of);
				if (c == '\0')
				{
					c = buffer[i + 1];
				}
				c = buffer[i];
			}

			// if (send(new_socket, buffer, PACKET_SIZE, 0) == -1)
			// {
			// 	perror("Error sending message!!!");
			// 	exit(EXIT_FAILURE);
			// }
			if ((sendto(server_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *)&clientAddress, cliLength)) == -1)
			{
				perror("Error sending message!!!");
				exit(EXIT_FAILURE);
			}
		}
		else
		{ // Packet corrupted
			printf("The packet was corrupted!\n");
			printf("\nMessage reads:\n%s(%lu bytes).", buffer, sizeof(buffer));
			buffer[10] = 'N';
			if ((sendto(server_fd, buffer, PACKET_SIZE, 0, (struct sockaddr *)&clientAddress, cliLength)) == -1)
			{
				perror("Error sending message!!!");
				exit(EXIT_FAILURE);
			}
		}
	}

	file.close();
}
