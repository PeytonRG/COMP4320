// Server side C/C++ program to demonstrate Socket programming
// https://www.geeksforgeeks.org/udp-server-client-implementation-c/?ref=lbp
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

int sockfd;
struct sockaddr_in cliaddr, servaddr;
char buffer[PACKET_SIZE] = {0};
char *hello = "Hello from server";

int connectToClient()
{
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if (::bind(sockfd, (const struct sockaddr *)&servaddr,
			   sizeof(servaddr)) < 0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	return 0;
}

int calculateChecksum(char packet[])
{
	int checksum = 0;
	// 7 is the first index of the message body
	for (int i = 7; i < 128; i++)
	{
		checksum += packet[i];
	}
	return checksum;
}

int receiveMessage()
{
	int packetNum = 1;
	while (true)
	{
		socklen_t socketLength;
		int msgLength;

		socketLength = sizeof(cliaddr); //len is value/resuslt

		msgLength = recvfrom(sockfd, (char *)buffer, PACKET_SIZE,
							 MSG_WAITALL, (struct sockaddr *)&cliaddr,
							 &socketLength);
		buffer[msgLength] = '\0';
		cout << "Packet #" << packetNum << " received" << endl;

		cout << "Calculating checksum for packet #" << packetNum << endl;
		cout << "Checksum: " << calculateChecksum(buffer) << endl;

		printf("Client : %s\n", buffer);
		packetNum++;
		sendto(sockfd, (const char *)hello, strlen(hello),
			   0, (const struct sockaddr *)&cliaddr,
			   socketLength);
		printf("Hello message sent.\n");
	}

	return 0;
	// // Variable declarations
	// char c;
	// int packetNumber = 1;
	// ofstream file;
	// file.open("TestFile");

	// // Checks if file opened properly
	// if (!file.is_open())
	// {
	// 	printf("Can't open output file");
	// 	exit(EXIT_FAILURE);
	// }

	// // int n = 240 * 1024;
	// // setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &n, sizeof(n));

	// while (c != '*')
	// {
	// 	int passed;
	// 	if (recvfrom(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *)&cliaddr, &cliLength) == -1)
	// 	{
	// 		perror("Error receiving message!!!");
	// 		exit(EXIT_FAILURE);
	// 	}

	// 	printf("\n\nReceived packet %d...\n", packetNumber);
	// 	packetNumber++;
	// 	printf("The packet is being checked for errors...\n");
	// 	passed = calculateChecksum(buffer);
	// 	if (passed == 1)
	// 	{
	// 		printf("The packet was valid!\n");
	// 		printf("Message reads:\n%s(%lu bytes).", buffer, sizeof(buffer));
	// 		int i = 0;
	// 		// fflush(of);
	// 		for (i = 11; i < PACKET_SIZE; i++)
	// 		{
	// 			file << buffer[i];
	// 			// putc(buffer[i], of);
	// 			if (c == '\0')
	// 			{
	// 				c = buffer[i + 1];
	// 			}
	// 			c = buffer[i];
	// 		}

	// 		// if (send(new_socket, buffer, PACKET_SIZE, 0) == -1)
	// 		// {
	// 		// 	perror("Error sending message!!!");
	// 		// 	exit(EXIT_FAILURE);
	// 		// }
	// 		if ((sendto(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *)&cliaddr, cliLength)) == -1)
	// 		{
	// 			perror("Error sending message!!!");
	// 			exit(EXIT_FAILURE);
	// 		}
	// 	}
	// 	else
	// 	{ // Packet corrupted
	// 		printf("The packet was corrupted!\n");
	// 		printf("\nMessage reads:\n%s(%lu bytes).", buffer, sizeof(buffer));
	// 		buffer[10] = 'N';
	// 		if ((sendto(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr *)&cliaddr, cliLength)) == -1)
	// 		{
	// 			perror("Error sending message!!!");
	// 			exit(EXIT_FAILURE);
	// 		}
	// 	}
	// }

	// file.close();
}

int main(int argc, char const *argv[])
{
	connectToClient();

	receiveMessage();

	return 0;
}
