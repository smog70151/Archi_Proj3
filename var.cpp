#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
// to include the var
#include "var.h"

using namespace std;

//output file
fstream snapshot;    // snapshot.rpt
fstream error_dump;  // error_dump.rpt
fstream report;      // report.rpt

//input file
fstream iimage;      // read iimage
fstream dimage;      // read dimage

//reg part
Register reg[32];
Register HI, LO, PC;

//Instruction and Data memory storage
unsigned int inst_mem[256]; //iimage max 1KB
unsigned int data_mem[1024]; //dimage max 1KB

//cyc
int cyc;

//Decode var
unsigned int address; //26 bits
unsigned int immediate; //16 bits
unsigned int opcode, funct; //6 bits
unsigned int rt, rs, rd, shamt; //5 bits

//Reg var
//data1 = rs, data2 = rt
int read_data1, read_data2; //to calculate
int simmediate; //signed immediate 16 -> 32

//Error
int error_halt; //D memory OVF or Misaligned occur
bool flag_OVW; //to detect HI, LO OVW

/* I Section (PC) */
int ICache_hit, ICache_miss;
int ITLB_hit, ITLB_miss;
int IPTE_hit, IPTE_miss;
/* D Section (Data) */
int DCache_hit, DCache_miss;
int DTLB_hit, DTLB_miss;
int DPTE_hit, DPTE_miss;

// TLB
TLB* ITLB;
int IPage_offset, IVPN;
int ITLB_tag, ITLB_index, ITLB_entries;
TLB* DTLB;
int DPage_offset, DVPN;
int DTLB_tag, DTLB_index, DTLB_entries;

// PTE
// Instruction
PTE* IPTE;
int IPage_size, IPTE_entries; // Default 8 bytes
// Data
PTE* DPTE;
int DPage_size, DPTE_entries;

// Disk
int IDisk_size; // Default 1024 bytes
int DDisk_size; // Default 1024 bytes

// Memory
// Instruction
Memory* IMemory;
int IMemory_size; // Default 64 bytes
int IMemory_entries;
// Data
Memory* DMemory;
int DMemory_size;
int DMemory_entries;

// Cache
Cache* ICache;
int ICache_size, IBlock_size; // Default 16 bytes, 4 bytes
int Iasscoiate; // Default 4-way
int IBlock_offset, IByte_offset;
int ICache_index, ICache_row;
int ICache_offset;
// Data
Cache* DCache;
int DCache_size, DBlock_size;
int Dasscoiate;
int DBlock_offset, DByte_offset;
int DCache_index, DCache_row;
int DCache_offset;
