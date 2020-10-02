#include <iostream>
#include <math.h>
#include <vector>
#include <random>
#include <ctime>
#include<chrono>
#include<thread>

void createRandomMessage(std::vector<bool>& message)
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	std::rand();

	for (auto&& i: message)
	{
		i = std::rand() % 2;
	}

}

int power(const int a, const int b)
{
	int answer{1};
	for (int i{}; i < b; i++)
		answer = answer * a;

	return answer;
}

int numberOfParityBits(const int m)
{
	// number of parity bits, p, required: 2^p >= p + m + 1
	int p{ 1 };

	while (power(2, p) - p < m + 1)
		p++;

	return p;
}

void printMessage(const std::vector<bool>& message)
{
	for (const auto i : message)
		std::cout << i << ' ';

	std::cout << '\n';
}

int checkCodeParity(const std::vector<bool>& code)
{
	int codeParity{};
	for (const auto bit : code)
		if (bit)
			codeParity = (codeParity + 1) % 2;

	return codeParity;
}

std::vector<bool> createCode(const std::vector<bool>& message, const int messageLength)
{
	int codeLength{ numberOfParityBits(messageLength) + messageLength + 1 };

	std::vector<bool> code(codeLength);

	//Insert message into the code
	//Need to skip parity bit spaces: 1, 2, 4, 8 etc as well as 0

	int codePosition{ 1 };										//keeps track of the current bit in the code. Skip zero, start at one
	int parityBitCount{ 0 };									//keeps track of the number of parity bits that have been skipped so far

	for (auto bit : message)									//for each bit in the message...
	{
		while (codePosition == power(2, parityBitCount))		//check if the current position is for a parity bit
		{
			codePosition++;										//if so, skip this position...
			parityBitCount++;									//and add one to the parity bit count
		}
			code[codePosition] = bit;							//if not, insert the message bit into the current position...
			codePosition++;										//and move to the next position for the next message bit
	}

	//Determine which parity bits should be turned on
	//take the position in the code of each bit with a value of 1 and use XOR operator with the current result.


	int result{};
	int bitNumber{};
	for (const auto bit : code)
	{
		result = result ^ (bit * bitNumber);
		bitNumber++;
	}

	//the final result in binary tells which parity bits should be turned on
	//eg if the result is ...00101, the 2nd and 3rd parity bits (in positions 1 and 4 in the code) should be turned on

	//Set parity bits as described above
	//For each parity bit use bitwise AND operator with the parity bit position in the code (1, 2, 4 etc) and the result from above
	//If the result is 1, the bit should be set to 1.

	for (int parityBit{ 1 }; parityBit < code.size(); parityBit <<= 1)
		if (parityBit & result)
		{
			code[parityBit] = 1;
			//std::cout << "Parity bit " << parityBit << " set to 1" << '\n';
		}

	//Set 0th bit to make the parity of the whole code even
	//If there are two errors, this will allow them to be detected, though their positions won't be known
	//Determine current parity of the code
	
	if (checkCodeParity(code))
		//If the parity is odd, set 0th bit to 1 to make whole code even
		code[0] = 1;

	return code;
}

void addError(std::vector<bool>& codeCopy)
{
	int randomBit{ std::rand() % static_cast<int>(codeCopy.size() + 1) };
	
	std::cout << "(Bit " << randomBit << " changed.)\n";

	codeCopy[randomBit] = (codeCopy[randomBit] + 1) % 2;

}

void checkError(const std::vector<bool>& code)
{
	int result{};
	int bitNumber{};
	for (const auto bit : code)
	{
		result = result ^ (bit * bitNumber);
		bitNumber++;
	}

	if (result)
		if (!checkCodeParity(code))
		{
			std::cout << "\nThere are two or more errors in the code at unknown positions.\n";
		}
		else
			std::cout << "\nThere is an error at position " << result << ". This will be corrected.\n";
	else
		std::cout << "\nThere are no errors in the code.\n";

}


int main()
{
	int messageLength{11};
	//std::cout << "Enter length of code: ";
	//std::cin >> n;

	std::vector<bool> message(messageLength);

	createRandomMessage(message);
	std::cout << "Initial message:\n";
	printMessage(message);

	std::vector<bool> code{ (createCode(message, messageLength)) };
	std::cout << "\nMessage with code added:\n";
	printMessage(code);

	std::vector<bool> codeCopy(code);

	addError(codeCopy);
	if (std::rand() % 2 == 0)
		addError(codeCopy);
	std::cout << "\nCode with bits changed:\n";

	printMessage(codeCopy);

	checkError(codeCopy);


	return 0;
}
