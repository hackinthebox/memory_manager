#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//define physical page size in kb
#define PAGE_SIZE 256
#define FRAME_COUNT 256
#define NUM_ARRAY 1000
#define BACKING_STORE "BACKING_STORE.bin"
#define OUTFILE "output.txt"
#define TLB_SIZE 16

struct page_struct { 
	char page[PAGE_SIZE];
};

struct tlb_entry {
	int heat;
	int page_num;
	int frame_num;
};

int vaddr_array[NUM_ARRAY];
int page_table[FRAME_COUNT];
int page_table_count = 0;

struct tlb_entry tlb_array[TLB_SIZE];
struct page_struct memory[FRAME_COUNT];

int readln(char *filename) {
	FILE *f;
	f = fopen(filename, "r");
	int count = 0;
	if ( f!=0 ) {
		while (fscanf(f, "%d", &vaddr_array[count]) > 0 && count < NUM_ARRAY) {
			count++; 
	}} else { 
		printf( "File %s could not be opened!\n", filename);
	
	}
	printf("[INFO] - Successfully read %d lines from file %s\n\n", count, filename);
	fclose(f);
	return count;
}

int pagenum(int vadd) {
	//the page number is the most significant  4 bits, so bit shift 4 right to get just the page number
	int mask = 65280, page = 0; // 0000 0000 0000 0000 1111 1111 0000 0000
	page = mask & vadd;
//	printf("[page debug] and result : %d", page);
	page = page >> 8;
//	printf("[page debug] bit shift  : %d", page);
	return page;
}


int offsetnum(int vadd) { 
	//offset is the least significant 4 bits
	//mask the rest
	int mask = 255, offset = 0; // 0000 0000 0000 0000 0000 1111 1111
	offset = vadd & mask;
	return offset;	
}

int pagelookup(int page) { 
	int frame;
	//check page table if page is already loaded into memory
	//pull page if not
	if(page_table[page] == 0 ) { 
		loadpage(page);
	}
	frame = page_table[page];
	return frame;
}
		

int loadpage(int page) {
	//load the page from the backing store
	FILE *f;
	f = fopen(BACKING_STORE, "rb");
	char *buf = memory[page_table_count].page;
	int seek;
	if ( f == 0 ) {
		printf( "Cannot open file: %s\n", BACKING_STORE);
	} else {
	//find the page from backing store
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
		
char *readbyte(int offset, int frame) {
	//read the byte value at the offset into the page
	char *loc = memory[frame].page;
	char *c = loc + offset;
	return c;
}

int main(int argc, char **argv) { 
	char *f = argv[1];
	int vadd=0, count=0, padd, i=0, frame, offset, page;
	char *value;
	FILE *o;
	o = fopen(OUTFILE, "w");
	//Read all the v-addresses into memory
	count = readln(f);
	//For each address in memory print the page#, physical address and, value
	for (i; i < count; i++) {
		//read address from array
		vadd = vaddr_array[i];
		//get the page #
		page = pagenum(vadd);
		//get the offset 
		offset = offsetnum(vadd);
		//load the page into memory from backing store, return memory base address for the page
		frame = pagelookup(page);
		//calculate physical address, base address + offset
		padd = (frame*256)+offset;
		//set char pointer to byte at physical address
		value = readbyte(offset, frame);
		fprintf(o, "Virtual address: %d Physical address: %d Value: %d\n",vadd, padd, *value);
		}
}

