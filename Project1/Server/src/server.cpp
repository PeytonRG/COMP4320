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
char *hello = "Hello from server";

int init()
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
	for (int i = 7; i < PACKET_SIZE; i++)
	{
		checksum += packet[i];
	}
	return checksum;
}

bool validateChecksum(char buffer[])
{
	int calculatedChecksum = calculateChecksum(buffer);
	// cout << "Checksum: " << calculateChecksum(buffer) << endl;

	string checkSumString;
	for (int i = 2; i < 7; i++)
	{
		checkSumString += buffer[i];
	}
	int passedChecksum = stoi(checkSumString);

	return calculatedChecksum == passedChecksum;
}

/**
 * Write the contents of the buffer's body to the file
 * @param file Pointer to the output file stream to write to.
 * @param buffer The buffer with the message to write to the file.
 */
int writeFile(ofstream &file, char buffer[])
{
	for (int i = 7; i < PACKET_SIZE; i++)
	{
		if (buffer[i] != '\0')
			file << buffer[i];
	}
}

int receiveMessage()
{
	char buffer[PACKET_SIZE] = {0};
	int packetNum = 1;
	ofstream file;
	socklen_t socketLength;
	int msgLength;

	socketLength = sizeof(cliaddr); //len is value/resuslt
	file.open("OutputFile.txt");

	// Checks if file opened properly
	if (!file.is_open())
	{
		printf("Can't open output file");
		exit(EXIT_FAILURE);
	}

	while (true)
	{
		msgLength = recvfrom(sockfd, (char *)buffer, PACKET_SIZE,
							 MSG_WAITALL, (struct sockaddr *)&cliaddr,
							 &socketLength);
		buffer[msgLength] = '\0';
		cout << "Packet #" << packetNum << " received" << endl;
		if (buffer[0] != '\0')
		{
			// first byte is seq #
			cout << "Sequence #: " << buffer[0] << endl;
			cout << "Calculating checksum for packet #" << packetNum << endl;
			if (validateChecksum(buffer))
			{
				cout << "Checksums matched" << endl;
			}
			else
			{
				cout << "Packet #" << packetNum << " is lost or damaged." << endl;
			}
		}
		// last packet. break out of the loop to close the ofstream
		else
			break;

		// print out the first 48 bytes of the body
		for (int i = 7; i < 56; i++)
		{
			cout << buffer[i];
		}
		cout << endl;

		writeFile(file, buffer);

		packetNum++;
		sendto(sockfd, (const char *)hello, strlen(hello),
			   0, (const struct sockaddr *)&cliaddr,
			   socketLength);
		printf("Hello message sent.\n");
	}
	file.close();
	sendto(sockfd, (const char *)hello, strlen(hello),
		   0, (const struct sockaddr *)&cliaddr,
		   socketLength);
	printf("Final message sent.\n");

	return 0;
}

int main(int argc, char const *argv[])
{
	init();

	receiveMessage();

	return 0;
}
