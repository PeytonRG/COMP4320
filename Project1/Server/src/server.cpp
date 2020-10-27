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

int init()
{
	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("Failed to create socket.");
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
		perror("Failed to bind socket.");
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
	try
	{
		int calculatedChecksum = calculateChecksum(buffer);

		string checkSumString;
		for (int i = 2; i < 7; i++)
		{
			checkSumString += buffer[i];
		}
		int passedChecksum = stoi(checkSumString);

		return calculatedChecksum == passedChecksum;
	}
	// the first character in the passed checksum is not an int
	catch (std::invalid_argument)
	{
		return false;
	}
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
	ofstream file;
	socklen_t socketLength;
	int msgLength;

	socketLength = sizeof(cliaddr); //len is value/resuslt
	file.open("OutputFile.txt");

	// Checks if file opened properly
	if (!file.is_open())
	{
		cout << "Failed to open output file." << endl;
		exit(EXIT_FAILURE);
	}

	char previousSequenceNumber;
	while (true)
	{
		msgLength = recvfrom(sockfd, (char *)buffer, PACKET_SIZE,
							 MSG_WAITALL, (struct sockaddr *)&cliaddr,
							 &socketLength);
		buffer[msgLength] = '\0';
		char sequenceNumber = buffer[0];

		if (sequenceNumber == '\0')
		{
			// This is the last packet. Break out to close the file stream.
			break;
		}
		else if (sequenceNumber == previousSequenceNumber)
		{
			cout << "Packet " << sequenceNumber << " has a duplicate sequence number. A packet was lost." << endl;
		}
		else
		{
			cout << "Packet " << sequenceNumber << " received. Checking for errors..." << endl;
			// checksum validation fails or the sequence number is invalid
			if (!validateChecksum(buffer) || (sequenceNumber != '0' && sequenceNumber != '1'))
			{
				cout << "Packet " << sequenceNumber << " is damaged." << endl;
			}
			else
			{
				cout << "Packet " << sequenceNumber << " is ok." << endl;
			}
		}

		// update the previous number to detect future packet loss
		previousSequenceNumber = sequenceNumber;

		cout << "Printing the first 48 bytes of packet " << sequenceNumber << ":" << endl;
		// print out the first 48 bytes of the body
		for (int i = 0; i < 48; i++)
		{
			cout << buffer[i];
		}
		cout << endl;

		writeFile(file, buffer);

		char resp[18];
		sprintf(resp, "Packet %c received.", sequenceNumber);

		sendto(sockfd, (const char *)resp, strlen(resp),
			   0, (const struct sockaddr *)&cliaddr,
			   socketLength);
		cout << "Sending: " << resp << endl;
		cout << endl;
	}
	cout << "Final packet received. Writing file." << endl;
	file.close();

	char successMsg[] = "PUT successfully completed";
	sendto(sockfd, (const char *)successMsg, strlen(successMsg),
		   0, (const struct sockaddr *)&cliaddr,
		   socketLength);
	cout << "Sending: " << successMsg << endl;

	return 0;
}

int main(int argc, char const *argv[])
{
	init();

	receiveMessage();

	return 0;
}
