// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#define PORT 8080
#define TESTFILE "./src/TestFile3"
using std::cout;
using std::endl;

int sock;
int valread;
std::stringstream buffer;
int damageProb;
int lossProb;

// connect to the client
int connect(const char* ipadr) {
	int sock = 0;
	struct sockaddr_in serv_addr;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		cout << "Socket creation error" << endl;
		return -1;
	}

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);


	// Convert IPv4 and IPv6 addresses from text to binary form
	if (inet_pton(AF_INET, ipadr, &serv_addr.sin_addr) <= 0)
	{
		cout << "Invalid address/ Address not supported" << endl;
		return -1;
	}

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		cout << "Connection failed" << endl;
		return -1;
	}

	return sock;
}

// calculate checksum by summing bytes of the packet
void calculateChecksum(char packet[], int packetCount) {
    int checksum = 0;
	for (int i = 0; i < 128; i++) {
		checksum += packet[i];
	}

	// place in packet
	char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	packet[2] = digits[checksum / 1000 % 10];
	packet[3] = digits[checksum / 100 % 10];
	packet[4] = digits[checksum / 10  % 10];
	packet[5] = digits[checksum % 10];

	cout << "Packet #" << packetCount << " checksum: " << std::to_string(checksum) << endl;
}

int getGremlinProbabilities() {
	cout << "Enter probability for packet damage (0-100): ";
	std::cin >> damageProb;
	cout << "Enter probability for packet loss (0-100): ";
	std::cin >> lossProb;
	cout << "Gremlin probabilities are (" << std::to_string(damageProb) << "% damage) and (" << std::to_string(lossProb) << "% loss)" << endl;
}

int gremlin(char packet[]) {
	return 0;
}

void sendPacket(char packet[], int packetCount) {
	// server is cutting it short atm
	// send(sock, packet, strlen(packet), 0);
	cout << "Packet #" << std::to_string(packetCount) << " sent" << endl;
}

// create packets
int createPackets() {
	int packetCount = 1;
	int totalCharCount = 0;
	char sequenceNum = '0';
	int headerSize = 6;

	while (totalCharCount < buffer.str().length()) {
		char packet[128];
		int charCountInBuffer = headerSize; // start after header

		// setup header
		packet[0] = sequenceNum;
		packet[1] = 'A'; // Error protocol: A if OK, B if ERROR
		// packet[2] Checksum 1000's
		// packet[3] Checksum 100's
		// packet[4] Checksum 10's
		// packet[5] Checksum 10's 

		cout << "writing data to packet #" + std::to_string(packetCount) << endl;

		// loop until packet is full or buffer is completely read
		while (totalCharCount < buffer.str().length() && charCountInBuffer < 128) {
			packet[charCountInBuffer] = buffer.str()[totalCharCount];
			charCountInBuffer++;
			totalCharCount++;
		}

		// check if packet is full and fill with null
		while (charCountInBuffer < 128) {
			packet[charCountInBuffer] = NULL;
			charCountInBuffer++;
		}

		// calculate checksum and add to packet
		calculateChecksum(packet, packetCount);
		
		// send packet to gremlin
		int gremlinValue = gremlin(packet);

		// show packet info
		std::string packetString = "";
		for (int i = 0; i < 128; i++) {
			packetString += packet[i];
		}
		cout << "Packet #" << std::to_string(packetCount) << " to be sent: " << packetString << endl;

		// send to server
		sendPacket(packet, packetCount);

		// recieve from client
		
		// check for error

		// update count and seqnum
		packetCount++;
		sequenceNum = (sequenceNum == '0') ? '1' : '0';
	}
	
	cout << "All packets sent" << endl;
	return 0;
}

// read test file into buffer
bool readFile(std::string fileName) {
	std::fstream file(fileName);
	if (!file.is_open()) { 
		cout << "File could not be found" << endl;
		return false;
	}

	buffer << file.rdbuf();
	cout << buffer.str().length() << endl;
	return true;
}

int main(int argc, char const *argv[])
{
	// char *hello = "Hello from client";
	// char *buffer[1024];
	
	sock = connect("127.0.0.1");
	if (sock < 0) { return -1; }

	if(!readFile(TESTFILE)) { return -1; }

	// get gremlin probabilities
	getGremlinProbabilities();

	// send packets
	createPackets();

	// recieve packets
	// int valread = read(sock, buffer, 1024);
	// cout << buffer << endl;
	
	return 0;
}