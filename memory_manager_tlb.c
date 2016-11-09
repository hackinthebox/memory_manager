// Virtual memory manager with LFU-TLB ( Least Frequently Used Replacement TLB -- Like Linux :)
// [HackintheBox]

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Set Constants
#define PAGE_SIZE 256
#define FRAME_COUNT 256
#define NUM_ARRAY 1000
#define BACKING_STORE "BACKING_STORE.bin"
#define OUTFILE "output.txt"
#define TLB_SIZE 16

// Create structs for page table entries and tlb entries.
struct page_struct { 
	char page[PAGE_SIZE];
};

struct tlb_entry {
	int heat;
	int page_num;
	int frame_num;
};

// Initialize variables and counters 
int page_table_count = 0;
int tlb_hits = 0;
int tlb_table_count = 0;

// Initialize heap memory arrays for TLB, memory, page table and address store
struct tlb_entry tlb_array[TLB_SIZE];
struct page_struct memory[FRAME_COUNT];
int vaddr_array[NUM_ARRAY];
int page_table[FRAME_COUNT];


// -- TLB Functions --

// Replace the least used TLB entry or select one from random.
int tlb_heat_check() {
	int c;
	int ptr = (rand() %16);
	int base = tlb_array[ptr].heat;
	for (c = 0; c < TLB_SIZE; c++) {
		if (tlb_array[c].heat < base) {
			base = tlb_array[c].heat;
			ptr = c;
			}
		}
	return ptr;
}


// Check if page is present in the TLB 
int tlb_lookup(int page) {
	int ptr = -1;
	int c;
	for (c = 0; c < TLB_SIZE; c++) {
		if (page == tlb_array[c].page_num) {
			tlb_array[c].heat ++;
			ptr = tlb_array[c].frame_num;
			tlb_hits ++;
			break;
			}
		}
	return ptr;
}

// Add entry to the TLB
int tlb_entry(int page, int frame) { 
	int ptr;
	if (tlb_table_count < TLB_SIZE) { 
		ptr = tlb_table_count;
		tlb_table_count ++;
		} else { 
			ptr = tlb_heat_check();
			}
	tlb_array[ptr].page_num = page;
	tlb_array[ptr].frame_num = frame;
	tlb_array[ptr].heat = 0;
}


// Read in the addresses from the input file
int readln(char *filename) {
	FILE *f;
	f = fopen(filename, "r");
	int count = 0;
	if ( f!=0 ) {
		while (fscanf(f, "%d", &vaddr_array[count]) > 0 && count < NUM_ARRAY) {
			count++; 
	}} else { 
		printf( "File %s could not be opened!\n", filename);
		exit(1);	
	}
	printf("[INFO] - Successfully read %d lines from file %s\n\n", count, filename);
	fclose(f);
	return count;
}

// Find the page number from the virtual address
int pagenum(int vadd) {
	//Pages are 256k in size , so bit shift virtual address 4 right to get the page number
	int mask = 65280, page; // 0000 0000 0000 0000 1111 1111 0000 0000
	// AND virtual address against the bit mask -- (not actually needed as we bit shift next) 
	page = mask & vadd;
	// Shift 8 bits to the right, to give just the page number
	page = page >> 8;
	return page;
}


// Find the offset from the virtual address 
int offsetnum(int vadd) { 
	//offset is the least significant 4 bits
	//mask the rest
	int mask = 255, offset = 0; // 0000 0000 0000 0000 0000 1111 1111
	offset = vadd & mask;
	return offset;	
}

// Lookup the page number in the TLB , then the Page Table
int pagelookup(int page) { 
	int frame;
	int tlb = tlb_lookup(page);
	//check tlb for previous entry
	if (tlb == -1) {
	//check page table if page has already been loaded into memory
	if(page_table[page] == 0 ) {
		// Load page into page table 
		loadpage(page);
	}
	frame = page_table[page];
	tlb_entry(page, frame);
	} else {
	frame = tlb;
	}
	return frame;
}
		

// Load the page into memory from the backing store
int loadpage(int page) {
	//load the page from the backing store
	FILE *f;
	f = fopen(BACKING_STORE, "rb");
	char *buf = memory[page_table_count].page;
	int seek;
	if ( f == 0 ) {
		printf( "Cannot open file: %s\n", BACKING_STORE);
		exit(2);
	} else {
	//find the page in the  backing store
	seek = page * PAGE_SIZE;
	fseek(f, seek, SEEK_SET);
	// copy 256bytes into memory array at correct location
	fread(buf, PAGE_SIZE, 1, f);
	fclose(f);
	//update the page table
	page_table[page] = page_table_count;
	//increement counter 
	page_table_count++;
	}
}

// Read the byte value at the offset into the frame number	
char *readbyte(int offset, int frame) {
	//read the byte value at the offset into the page
	char *loc = memory[frame].page;
	char *c = loc + offset;
	return c;
}

int main(int argc, char **argv) {
	if (argc < 2){ 
		printf("No address file specified! exiting!\n");
		exit(3);
	}
	char *f = argv[1];
	srand((unsigned) time(0));
	int vadd, count, padd, i, frame, offset, page;
	char *value;
	FILE *o;
	o = fopen(OUTFILE, "w");
	//Read all the v-addresses into memory
	count = readln(f);
	//For each address in memory print the page#, physical address and, value
	for (i; i < count; i++) {
		//read address from array
		vadd = vaddr_array[i];
		//get the page number
		page = pagenum(vadd);
		//get the offset 
		offset = offsetnum(vadd);
		//load the page into memory from backing store, return memory base address for the page
		frame = pagelookup(page);
		//calculate physical address, base address + offset
		padd = (frame*256)+offset;
		//set char pointer to byte at physical address
		value = readbyte(offset, frame);
		//write physical address and value to output file
		fprintf(o, "Virtual address: %d Physical address: %d Value: %d\n",vadd, padd, *value);
		}	
	printf("There were %d TLB hits this run!\n", tlb_hits);
}
