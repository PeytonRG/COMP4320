#include <iostream>
using namespace std;

int multiply(int first, int second);

int main()
{
	cout << "Hello world!" << endl;
	int first, second;
	cout << "Enter first number: ";
	cin >> first;
	cout << "Enter second number: ";
	cin >> second;
	cout << multiply(first, second) << endl;
	return 0;
}

int multiply(int first, int second)
{
	return first * second;
}