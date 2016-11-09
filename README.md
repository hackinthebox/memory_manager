Designing a Virtual Memory Manager

This project consists of writing a program that translates logical to physical
addresses for a virtual address space of size 2 16 = 65,536 bytes. Your program
will read from a file containing logical addresses and, using a TLB as well as
a page table, will translate each logical address to its corresponding physical
address and output the value of the byte stored at the translated physical
address. The goal behind this project is to simulate the steps involved in
translating logical to physical addresses.

Specifics
Your program will read a file containing several 32-bit integer numbers that
represent logical addresses. However, you need only be concerned with 16-bit
addresses, so you must mask the rightmost 16 bits of each logical address.
These 16 bits are divided into (1) an 8-bit page number and (2) 8-bit page offset.
Hence, the addresses are structured as shown in Figure 9.33.
Other specifics include the following:

2 8 entries in the page table
Page size of 2 8 bytes
16 entries in the TLB
Frame size of 2 8 bytes
256 frames
Physical memory of 65,536 bytes (256 frames × 256-byte frame size)
