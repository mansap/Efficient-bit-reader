/* 
File Name: code_challenge.c
Title: Reading packed 12 bit unsigned integers from a binary file
       (test#.in) and finding the top 32 as well as last 32 values 
       read and writing them in an output file (test#.out) 
Author: Mansa Pabbaraju
*/


/**********Instructions to run********
1. To compile: make
2. To run: make ARGS="test1.bin test1.out" run
3. To delete executable: make clean
*/

/***** Concept and design decisions *****

As input file is a binary file, values are read byte-wise.
   Two main reasons: 
   1. Reading byte-wise would not cause any Endianess problem if
   on the host and network being different Endian formats
   2. It becomes easier to read the last set of values if there
   are odd number of 12 bits involved, depending on the pending previous
   value read, the last nibble can be effectively ignored
To unpack 12 bit values, byte shift operation is used either by 0, 4 or 8 bits
depending on byte being read and previous pending MSB. 
 
******** Top 32 values: Sorting ************* 
To maintain top 32 values, an array is used which is
sorted using insertion sort. As each 12 bit value is read and unpacked from 
binary file, the value is sent to insert() function which checks if this 
value needs to be added to the sorted array. There are three possiblities:
1. Number of elements read in the array is less than 32, so correct insert 
position is found and element added
2. Number of elements read in the array is 32, and new value is greater than
smallest value, so correct insert position is found and element added
3. Number of elements read in the array is 32, and new value is less than
smallest value, so it is ignored

****** latest 32 values, circular indexed array ***************
An index to find the correct position of latest value is found using the modulus
operator, each time the oldest read value is replaced by the newest read value

******* Time Complexity Analysis ******
1. Reading the file is O(n) where n is the number of bytes in file, as each byte 
needs to be read
2. Unpacking the values is O(1) as bit shifting is used for each byte depending 
on previous pending byte, a constant. number of operations
3. Sorting and storing the top 32 is O(32) as for each 12 bit value, its correct 
position is found in array and then inserted by shifting either left or right: 
O(32) + O(32) which is O(32), thus constant time
4. Finding the last 32 values is a single operation, O(1)

TOTAL TIME COMPLEXITY from above 4 steps:
O(n) + O(1) + O(32) + O(1) which is O(n),
where n is number of bytes in binary file

Design Decision: Also considered using a heap, which would be O(1) insert, 
and O(log n) remove but search would be O(n) to find correct position.
Also, need to output in sorted format hence decided to go with a 32 size
of unsigned short array, maintaining sorted order at every insert

Design Decision: Considered using structure to store three bytes in one, 
threby 12 bytes. However, depending of microcontroller, the padding may vary 
and there may be wastage of space. Also, Endianess while reading 3 bytes 
together was a major concern.

******* Space Complexity Analysis ******
1. There are two unsigned short arrays of size 32. Converting 12 bits to unsigned
integers requires atleast and at most 16 bits, as 2^11 is highest value possible.
Using a short guarantees atleast 16 bits and depending on processor, 
it is lesser than 32 bits.O(32) + O(32) space
2. curr_idx, last_idx which are used to store current indices of above arrays are 
unsigned char so they use only 1 byte, also bits_msb. 
Each of the local variables used are either unsigned char or unsigned short, 
unsigned char is used wherever possible.
Hence, total space complexity is O(32).

*/
#include <stdio.h>
/* Macro for the top k 12-bit values, here it is 32
Code can be generalised upto top TOP 2^11 12-bit values values 
by changing this macro */
#define TOP_K 32

/* Global variables: Choice of global use and maintain values between 
 consecutive byte reads from binary file */

/* A global variable storing pending 4 or 8 bit value from 
previous byte read */
unsigned short prev_msb = 0;
/* number of pending bits read, either 4 or 8 */
unsigned char bits_msb = 0;
/* Array to store the top 32 values*/
unsigned short top_values[TOP_K];
/* Array to store the last 32 values*/
unsigned short last_values[TOP_K];
/* curr_idx denotes the number of values read, if less than 32 */
unsigned char curr_idx = 0;
/* last_idx is used to implement a circular loop on a simple array, 
enabling to store the latest 32 values*/
unsigned char last_idx = 0;


/* Function Declarations */
// returns 1 if file not read else 0
unsigned char read_file(char* filename);
void unpack_bytes(unsigned char byte);

void insert(unsigned short val);



int main(int argc, char* argv[])
{
	char* binary_file = NULL;
	char* output_file = NULL;
	unsigned char idx = 0;
	FILE* fp_write = NULL;
	/* Check for number of arguments, throw error and return if incorrect*/
	if(argc != 3)
	{
		printf("%s\n","Incorrect number of inputs, exiting..." );
		return 1;
	}
	/* Read first argument as input file */
	binary_file = argv[1];
	//binary_file = "test1.bin";
	/* Read second argument as output file */
	output_file = argv[2];
	//output_file = "res.out";
	/* array initialization to be safe from garbage values*/
	for(idx = 0; idx < TOP_K; idx++)
	{
		top_values[idx] = 0;
	}

	/* return with code 1 if file could not be opened to read */
	if(read_file(binary_file) == 1)
	{
		return 1;
	}

	fp_write = fopen(output_file, "w");
	/* return with code 1 if file could not be opened to write */
	if(fp_write == NULL)
	{
		fprintf(stderr, "Cannot open file to write, exiting...");
		return 1;
	}


	/* output the top 32 values */
	fprintf(fp_write, "%s\n","--Sorted Max 32 Values--");
	/* if there are less than 32 values in input file, 
	 print all of them */
	if(curr_idx < TOP_K)
	{
		for(idx = 0; idx < curr_idx; idx++)
		{
			fprintf(fp_write, "%hu\n",top_values[idx]);
		}
	}
	/* if there are 32 or more values in input file */
	else
	{
		for(idx = 0; idx < TOP_K; idx++)
		{
			fprintf(fp_write, "%hu\n",top_values[idx]);
		}
	}

	/* output the last 32 values */
	fprintf(fp_write,"%s\n","--Last 32 Values--");
	/* if there are less than 32 values in input file, 
	 print all of them */
	if(curr_idx < TOP_K)
	{
		for(idx = 0; idx < curr_idx; idx++)
		{
			fprintf(fp_write, "%hu\n",last_values[idx]);
		}
	}
	/* if there are 32 or more values in input file */
	else
	{
		/* start from oldest val, go uptill end */
		for(idx = last_idx; idx < TOP_K; idx++)
		{
			fprintf(fp_write, "%hu\n",last_values[idx]);
		}
		/* Now start from beginning and go uptill the latest value stored,
		 this is value at last_idx */
		for(idx = 0; idx < last_idx; idx++)
		{
			fprintf(fp_write, "%hu\n",last_values[idx]);
		}
	}
	fclose(fp_write);


	return 0;
}


unsigned char read_file(char* filename)
{
	unsigned int curr_read = 0;
	unsigned char byte =0;
	char count = 0;
	char num_bytes = 0;
	FILE* fp = NULL;

	fp = fopen(filename, "rb");
	/* if unsuccessful to open file, return */
	if(fp == NULL)
	{
		fprintf(stderr, "Cannot open file, exiting...");
		return 1;
	}
	/* successfully opened file */
	else
	{
		/* while there is a byte left, read it */
		while(fread(&byte,sizeof(char),1,fp) == 1)
		{
			unpack_bytes(byte);
		}	
	}
	fclose(fp);
	/* successfully read file */
	return 0;
}

void unpack_bytes(unsigned char byte)
{
	unsigned short value =0;
	unsigned char mask = 0;
	unsigned short temp =0;

	/* if reading first 8 bits of a 12 bit value */
	if(bits_msb == 0)
	{
		/* store whole byte as msb for next incoming */
		prev_msb = 0;
		prev_msb = byte;
		/* pending number of bits from previous byte is 8 */
		bits_msb = 8;
	}
	/* if reading last 8 bits of a 12 bit value */
	else if (bits_msb == 4)
	{
		/* create a 12 bit value */
		value = (prev_msb << 8) | byte;
		/* check if need to insert in sorted array and last_values array */
		insert(value);
		/* current byte completely read, no pending bits*/
		prev_msb = 0;
		/* pending number of bits from previous byte is 0 */
		bits_msb = 0;
	}
	/* if need to read last 4 bits of a 12 bit value ie
	already read first 8 bits, need the next 4 bits */
	else if (bits_msb == 8)
	{
		temp = prev_msb;
		mask = (1 << 4) - 1;
		prev_msb = 0;
		/* store remaining next 4 bits as pending msb  */
		prev_msb = byte & mask;
		byte = byte >> 4;
		value = (temp << 4) | byte;
		/* check if need to insert in sorted array and last_values array */
		insert(value);
		/* pending number of bits from previous byte is 4 */
		bits_msb = 4;
	}
}


void insert(unsigned short val)
{
	unsigned char insert_idx = 0;
	unsigned char idx = 0;
	/* if first value being read, insert it */
	if(curr_idx == 0)
	{
		top_values[curr_idx] = val;
		curr_idx = curr_idx +1;
	}
	else
	{
		/* if less than 32 values read or new value greater than smallest */
		if((curr_idx < TOP_K) || (top_values[0] < val)) 
		{
			/* find correct position to insert */
			while((idx < curr_idx) && (top_values[idx] < val))
			{
				idx = idx+1;
			}
			/* if insert position is at end, this is new largest,
			need to remove smallest from position 0 */
			if(idx == TOP_K)
			{
				insert_idx = TOP_K - 1;
			}
			else
			{
				insert_idx = idx;
			}
			/* need number of elements added is less than 32, we need to
			 add anyway, thereby increasing size of array */
			if (curr_idx < TOP_K) 
			{
				/* shift all elements from insert index to right by 1 */
				for(idx = curr_idx; idx > insert_idx ; idx--)
				{
					top_values[idx] = top_values[idx-1];
				}
				curr_idx = curr_idx + 1;
				top_values[insert_idx] = val;
			}
			/* there are already 32 elements sorted, new value is 
			 greater than smallest*/
			else
			{
				/* shift all elements uptill insert index to left by 1 */
				for(idx = 0; idx < insert_idx;idx++)
				{
					top_values[idx] = top_values[idx+1];
				}
				top_values[insert_idx] = val;
			}

		}
	}

	/* store last 32 values in an array, 
	which has a pointer running in a circular loop */
	last_values[last_idx] = val;
	last_idx = (last_idx + 1)%TOP_K;
}





