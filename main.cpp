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
	std::rand(); //discard first result

	for (auto&& i: message)
		i = std::rand() % 2;

}

int power(const int a, const int b)
{
	int answer{1};
	for (int i{}; i < b; i++)
		answer = answer * a;

	return answer;
}

int calculateCodeLength(const int m)
{
	// number of parity bits, p, required: 2^p >= p + m + 1, where m is the length of the message
	int p{ 1 };

	while (power(2, p) - p < m + 1)
		p++;

	return p + m + 1;
}

int calculateMessageLength(const int l)
{
	// number of parity bits, p, in a code of length l: 2^p >= l, 
	int p{ 1 };

	while (power(2, p) < l)
		p++;

	return l - (p + 1);
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
	int codeLength{ calculateCodeLength(messageLength) };

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
			code[parityBit] = 1;

	//Set 0th bit to make the parity of the whole code even
	//If there are two errors, this will allow them to be detected, though their positions won't be known
	//Determine current parity of the code
	
	if (checkCodeParity(code))
		//If the parity is odd, set 0th bit to 1 to make the whole code even
		code[0] = 1;

	return code;
}

void addError(std::vector<bool>& code)
{
	//choose a randon bit in the code
	int randomBit{ std::rand() % static_cast<int>(code.size()) };
	
	std::cout << "\nBit " << randomBit << " changed.";

	code[randomBit] = (code[randomBit] + 1) % 2;

}

int checkError(const std::vector<bool>& code)
{
	//take the bitwise XOR of the positions of each bit in the code that is turned on
	//if there are no errors, this will return 0, since the parity bits were set to esnure this
	//if there is exactly one error, it will return the position of the error.
	//if there are two or more errors, it will return a positive number, but the positions of the errors cannot be determined
	int errorPosition{};
	int bitNumber{};
	for (const auto bit : code)
	{
		errorPosition = errorPosition ^ (bit * bitNumber);
		bitNumber++;
	}

	//check if there are any errors - if there are no errors, errorPosition will be equal to zero
	if (errorPosition)
		//if there are errors, check the parity of the code whole code. If even (i.e. = 0), there must be two or more errors thanks to the use of the 0th bit
		if (!checkCodeParity(code))
		{
			std::cout << "\nThere are two or more errors in the code at unknown positions.\n";
			return 0;
		}
		
		//if the parity is odd, there is one error with the value of result being its position.
		else
		{
			std::cout << "\nThere is an error at position " << errorPosition << ".\n";
			return errorPosition;
		}
	else
	{
		std::cout << "\nThere are no errors in the code.\n";
		return 0;
	}
}

void correctError(const int errorPosition, std::vector<bool>& code)
{
	code[errorPosition] = (code[errorPosition] + 1) % 2;
}

void extractMessage(std::vector<bool>& code)
{
	//the length of the original message is not sent in the code, so need to calculate it:
	std::vector<bool> extractedMessage(calculateMessageLength(code.size()));
	
	int codePosition{ 0 };
	int messagePosition{ 0 };
	int parityBitCount{ 0 };

	for (auto bit : code)	
	{
		//0th bit is never a message bit, so skip
		if (codePosition == 0)
			codePosition++;
		//if the current bit is a parity bit, skip
		else if (codePosition == power(2, parityBitCount))	
		{
			codePosition++;
			parityBitCount++;	
		}
		//otherwise the current bit must be a message bit, so add it to extractedMessage
		else
		{
			extractedMessage[messagePosition] = bit;
			codePosition++;
			messagePosition++;
		}
	}
	std::cout << "\nThe final message with an error corrected if possible:\n";
	printMessage(extractedMessage);
}

int main()
{
	int messageLength{};
	std::cout << "Enter length of code: ";
	std::cin >> messageLength;

	//create a vector and generate a random message
	std::vector<bool> message(messageLength); 
	createRandomMessage(message);
	std::cout << "Initial message:\n";
	printMessage(message);

	//create a vector containing the encoded message
	std::vector<bool> code{ (createCode(message, messageLength)) };
	std::cout << "\nMessage with code added:\n";
	printMessage(code);

	//create a copy of the code and add one or two errors
	std::vector<bool> codeCopy(code);
	addError(codeCopy);
	if (std::rand() % 2 == 0)
		addError(codeCopy);
	std::cout << "\nCode with bits changed:\n";
	printMessage(codeCopy);

	//check the code for errors
	int errorPosition(checkError(codeCopy));
	if (errorPosition)
	{
		std::cout << "Code with error corrected:\n";
		correctError(errorPosition, codeCopy);
		printMessage(codeCopy);
	};

	//extract and print the message, either corrected or with two errors.
	extractMessage(codeCopy);

	return 0;
}
