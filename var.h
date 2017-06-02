#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#ifndef VAR_H
#define VAR_H


using namespace std;

//output file
extern fstream snapshot;    // snapshot.rpt
extern fstream error_dump;  // error_dump.rpt
extern fstream report;
extern fstream trace;
//input file
extern fstream iimage;      // read iimage
extern fstream dimage;      // read dimage

//reg part
typedef struct Register {
    int pre;
    int cur;
} Register;

extern Register reg[32];
extern Register HI, LO, PC;

//Instruction and Data memory storage
extern unsigned int inst_mem[256]; //iimage max 1KB
extern unsigned int data_mem[1024]; //dimage max 1KB

//cyc
extern int cyc;

//Decode var
extern unsigned int address; //26 bits
extern unsigned int immediate; //16 bits
extern unsigned int opcode, funct; //6 bits
extern unsigned int rt, rs, rd, shamt; //5 bits

//Reg var
//data1 = rs, data2 = rt
extern int read_data1, read_data2; //to calculate
extern int simmediate; //signed immediate 16 -> 32

//Error
extern int error_halt; //D memory OVF or Misaligned occur
extern bool flag_OVW; //to detect HI, LO OVW

/* I Section (PC) */
extern int ICache_hit, ICache_miss;
extern int ITLB_hit, ITLB_miss;
extern int IPTE_hit, IPTE_miss;
/* D Section (Data) */
extern int DCache_hit, DCache_miss;
extern int DTLB_hit, DTLB_miss;
extern int DPTE_hit, DPTE_miss;

// TLB
typedef struct TLB {
    bool valid;
    unsigned int LRU;
    unsigned int tag;
    unsigned int PPN;
} TLB;
// Instruction
extern TLB* ITLB;
extern int IPage_offset, IVPN;
extern int ITLB_tag, ITLB_index, ITLB_entries;
// Data
extern TLB* DTLB;
extern int DPage_offset, DVPN;
extern int DTLB_tag, DTLB_index, DTLB_entries;

// PTE
typedef struct PTE {
    bool valid;
    unsigned int LRU;
    unsigned int VPN;
    unsigned int PPN;
} PTE;
// Instruction
extern PTE* IPTE;
extern int IPage_size, IPTE_entries; // Default 8 bytes
// Data
extern PTE* DPTE;
extern int DPage_size, DPTE_entries;

// Disk
extern int IDisk_size; // Default 1024 bytes
extern int DDisk_size;

// Memory
typedef struct Memory {
    bool valid;
    unsigned int LRU;
} Memory;
// Instruction
extern Memory* IMemory;
extern int IMemory_size; // Default 64 bytes
extern int IMemory_entries;
// Data
extern Memory* DMemory;
extern int DMemory_size;
extern int DMemory_entries;

// Cache
typedef struct Set {
    bool valid;
    bool MRU;
    unsigned int tag;
} Set;
typedef struct Cache {
    Set* set;
} Cache;
// Instruction
extern Cache* ICache;
extern int ICache_size, IBlock_size; // Default 16 bytes, 4 bytes
extern int Iasscoiate; // Default 4-way
extern int IBlock_offset, IByte_offset, ICache_offset;
extern int ICache_index, ICache_row;
// Data
extern Cache* DCache;
extern int DCache_size, DBlock_size;
extern int Dasscoiate;
extern int DBlock_offset, DByte_offset, DCache_offset;
extern int DCache_index, DCache_row;

#endif // VAR_H
