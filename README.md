# hamming-code

A Hamming code is a simple error correction code capable of detecting and correcting a single bit error, or detecting (but not correcting) two errors, in a message or file. It works using parity bits, each of which is set so certain blocks in the message have even parity. The number of parity bits, p, required is determined by

2^p >= p + m + 1

where m is the number of bits in the message to be encoded.

Once the message is encoded, if there is an error (i.e. a bit is flipped) one or more blocks will have odd parity, and determining which allows the error to be corrected. This can be implemented using bitwise XOR operations.

This program creates a random message, encodes it, randomly changes one or two bits, and corrects a single bit or notes that two bits have been changed.
