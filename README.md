# Efficient-bit-reader
An application to unpack and pack data grouped logically in byte stream using an efficient bit masking technique independent of Endianess of underlying OS, a critical feature in memory constrained embedded devices
Note: In this application, specifically running for 12 bits, can configure changes to run for different bit size values.
Instructions to run:

1. To compile: make
2. To run: make ARGS="test1.bin test1.out" run
3. To delete executable: make clean

Design decisions:
As input file is a binary file, values are read byte-wise.
   Two main reasons: 
   1. Reading byte-wise would not cause any Endianess problem if
   on the host and network being different Endian formats
   2. It becomes easier to read the last set of values if there
   are odd number of 12 bits involved, depending on the pending previous
   value read, the last nibble can be effectively ignored
To unpack 12 bit values, byte shift operation is used either by 0, 4 or 8 bits
depending on byte being read and previous pending MSB. 




