#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdlib>
// to include the var
#include "var.h"
// to decode the instruction
#include "decode.h"
// to load the register data
#include "register.h"
// to implement the instruction
#include "alu.h"
// to detect the error
#include "error_detect.h"

using namespace std;

//Init in the beginning
void init();
//trans bytes to word
unsigned int c2i_inst_data(unsigned int inst, int pow,unsigned char value);
//read data
void read_iimage();
void read_dimage();
//Snapshot
void Snapshot();
/* Project 3 */
// initMemory
int offset(int value);
void initiMemory();
void initdMemory();
// ITLB ICache IPTE
unsigned int iTLB_Hit(unsigned int VPN); // Return PPN
void iTLB_Miss(unsigned int VPN, unsigned int PPN); // Write form TLB
unsigned int iPTE(unsigned int VPN);  // Return PPN for TLB
void iCache(unsigned int PA);
void iReport();
void memoryCheck(unsigned int VA);

int main(int argc, char **argv)
{
    unsigned int addr;
    //Init var
    init();
    //Init condition

    if( argc == 11 )
    {
        IMemory_size = atoi(argv[1]);
        DMemory_size = atoi(argv[2]);
        IPage_size = atoi(argv[3]);
        DPage_size = atoi(argv[4]);
        ICache_size = atoi(argv[5]);
        IBlock_size = atoi(argv[6]);
        Iasscoiate = atoi(argv[7]);
        DCache_size = atoi(argv[8]);
        DBlock_size = atoi(argv[9]);
        Dasscoiate = atoi(argv[10]);
    }
    else
    {
        IMemory_size = 64;
        DMemory_size = 32;
        IPage_size = 8;
        DPage_size = 16;
        ICache_size = 16;
        IBlock_size = 4;
        Iasscoiate = 4;
        DCache_size = 16;
        DBlock_size = 4;
        Dasscoiate = 1;
    }
    //Init memory
    initiMemory();
    initdMemory();
    //read *.bin1
    read_iimage();
    read_dimage();

    //simulation part
    while(1)
    {
        if(error_halt==1) break; //error detect 1.halt 2.error
        Snapshot();
        if(cyc==500001 || PC.cur > 1023) break; //cyc > 500,000
        cyc++; //cycle ++;
        addr = PC.cur/4 ; //load instruction memory
        memoryCheck(PC.cur);
        PC_ALU(); //PC = PC+4 -> next instruction
        Decode(inst_mem[addr]); //Decode current instruction
        Read_Reg(); //Read the red data and signed immediate( simmediate )
        ALU(); //Implement the instruction meaning
    }

    iReport();
    //close *.rpt
    snapshot.close();
    error_dump.close();
    report.close();
    return 0;
}

void init()
{
    //Init i-part and d-part mem
    for(int i=0; i<256; i++)
        inst_mem[i] = 0;
    for(int i=0; i<1024; i++)
        data_mem[i] = 0;
    //Init HI, LO, PC ;
    HI.pre = LO.pre = PC.pre = 0;
    HI.cur = LO.cur = PC.cur = 0;
    //Init Register
    for(int i=0; i<32; i++)
        reg[i].cur = reg[i].pre = 0;
    //Init cyc
    cyc = 0;
    //Init Error halt detect
    error_halt = 0;
    flag_OVW = true; // true -> correct

    snapshot.open("snapshot.rpt", ios::out);
    error_dump.open("error_dump.rpt", ios::out);
    report.open("report.rpt", ios::out);
}

void read_iimage()
{
    iimage.open("iimage.bin",ios::in|ios::binary);
    unsigned char c;
    unsigned int inst, count, temp;

    //Init PC addr (iimage_0)
    for(int i=0; i<4; i++)
    {
        if(i==0) inst = 0;
        iimage.read((char*)&c,sizeof(char));
        inst=c2i_inst_data(inst,4-i,c);
    }
    PC.cur = inst; // write into pc

    // numbers of instruction
    for(int i=0; i<4; i++)
    {
        if(i==0) inst = 0;
        iimage.read((char*)&c,sizeof(char));
        inst=c2i_inst_data(inst,4-i,c);
    }
    temp = count = inst; // following steps

    // instruction part
    while(temp)
    {
        //read the instruction part of the iimage file
        for(int i=0; i<4; i++)
        {
            if(i==0) inst = 0;
            iimage.read((char*)&c,sizeof(char));
            inst=c2i_inst_data(inst,4-i,c);
        }

        //store inst
        unsigned int addr = PC.cur/4 + count - temp ;
        if(addr>255)
        {
            cout << "illegal testcase" << endl;
            error_halt = 1;
            break;
        }
        inst_mem[addr]=inst;

        temp--;
    }
    iimage.clear();

}

void read_dimage()
{
    dimage.open("dimage.bin",ios::in|ios::binary);
    unsigned char c;
    unsigned int data, count, temp;

    //read $sp in the file (dimage_0)
    for(int i=0; i<4; i++)
    {
        if(i==0) data = 0;
        dimage.read((char*)&c,sizeof(char));
        data=c2i_inst_data(data,4-i,c);
    }
    //store the reg[29]($sp) data
    reg[29].cur=data;

    // numbers of data
    for(int i=0; i<4; i++)
    {
        if(i==0) data = 0;
        dimage.read((char*)&c,sizeof(char));
        data=c2i_inst_data(data,4-i,c);
    }
    temp = count = data; // following data

    while(temp)
    {
        //read the rest part of the dimage file
		for(int i=0; i<4; i++)
		{
			if(i==0) data = 0;
			dimage.read((char*)&c,sizeof(char));
			data=c2i_inst_data(data,4-i,c);
		}

		//store mem
		unsigned int addr = 4*(count - temp);
		if(addr+3>1023)
        {
            cout << "illegal testcase" << endl;
            error_halt = 1;
            break;
        }
		data_mem[addr]   = (data&0xff000000)>>24;
		data_mem[addr+1] = (data&0x00ff0000)>>16;
		data_mem[addr+2] = (data&0x0000ff00)>>8 ;
		data_mem[addr+3] = (data&0x000000ff)    ;

		temp--;
    }
    dimage.close();

}

unsigned int c2i_inst_data(unsigned int inst, int pow,unsigned char value)
{
    if(pow==4)
        inst=(value>>4<<28)+((value%16)<<24);
    else if(pow==3)
        inst+=(value>>4<<20)+((value%16)<<16);
    else if(pow==2)
        inst+=(value>>4<<12)+((value%16)<<8);
    else if(pow==1)
        inst+=(value>>4<<4)+value%16;
    return inst;
}

void Snapshot()
{
    snapshot << "cycle " << dec << cyc << endl;
    //cout << "cyc : " << dec << cyc << endl;
    if(cyc==0)
    {
        //1st cyc snapshot  reg -> HI -> LO -> PC
        for(int i = 0; i<32 ; i++)
            snapshot << "$" << setw(2) << setfill('0') << dec << i << ": 0x" << setw(8) << setfill('0') << hex << uppercase << reg[i].cur << endl;
        snapshot << "$HI: 0x" << setw(8) << setfill('0') << hex << uppercase << HI.cur << endl;
        snapshot << "$LO: 0x" << setw(8) << setfill('0') << hex << uppercase << LO.cur << endl;
        snapshot << "PC: 0x" << setw(8) << setfill('0') << hex << uppercase << PC.cur << endl;
    }
    else
    {
        //snapshot the altered reg
        for(int i = 0; i<32; i++)
            if(reg[i].pre != reg[i].cur)
                snapshot << "$" << setw(2) << setfill('0') << dec << i << ": 0x" << setw(8) << setfill('0') << hex << uppercase << reg[i].cur << endl;
        if(HI.pre!=HI.cur)
            snapshot << "$HI: 0x" << setw(8) << setfill('0') << hex << uppercase << HI.cur << endl;
        if(LO.pre!=LO.cur)
            snapshot << "$LO: 0x" << setw(8) << setfill('0') << hex << uppercase << LO.cur << endl;
        if(PC.pre!=PC.cur)
            snapshot << "PC: 0x" << setw(8) << setfill('0') << hex << uppercase << PC.cur << endl;
    }
    snapshot << endl << endl;
    for(int i = 0; i<32 ; i++)
        reg[i].pre = reg[i].cur;
    HI.pre = HI.cur;
    LO.pre = LO.cur;
    PC.pre = PC.cur;
}

/* CMP */

unsigned int offset(unsigned int value)
{
    int i = 0;
    int temp = value;
    while (temp/2) {
        temp = temp/2;
        i++;
    }
    return i;
}

void initiMemory()
{
    // Disk
    IDisk_size = 1024;

    // PTE
    IPTE_entries = IDisk_size / IPage_size;

    // TLB
    ITLB_entries = IPTE_entries / 4;
    ITLB_index = offset((unsigned int)IPTE_entries);

    // Page offset
    IPage_offset = offset((unsigned int)IPage_size);

    // Cache Offset
    IByte_offset = 2;
    ICache_row = ICache_index = ICache_size/IBlock_size/Iasscoiate;
    IBlock_offset = offset((unsigned int)ICache_index);
    ICache_offset = IBlock_offset;
    // cout << "ICache_offset : " << dec << setw(2) << ICache_offset << endl;

    // Memory
    IMemory_entries = IMemory_size/IPage_size;
    // cout << "PTE : " << dec <<IPTE_entries << endl;
    // cout << "Memory : " << dec <<IMemory_entries << endl;
    IMemory = new Memory[IMemory_entries];
    for(int i = 0; i < IMemory_entries; i++)
    {
        IMemory[i].valid = false;
        IMemory[i].LRU = 0;
    }


    // PTE
    IPTE = new PTE[IPTE_entries];
    for(int i = 0; i < IPTE_entries; i++)
    {
        IPTE[i].valid = false;
        IPTE[i].LRU = IPTE[i].PPN = 0;
    }

    // TLB
    ITLB = new TLB[ITLB_entries];
    for(int i = 0; i < ITLB_entries; i++)
    {
        ITLB[i].valid = false;
        ITLB[i].tag = ITLB[i].LRU = ITLB[i].PPN = 0;
    }

    // Cache
    ICache = new Cache[ICache_index];
    for(int i = 0; i < ICache_index; i++)
    {
        ICache[i].set = new Set[Iasscoiate];
        for(int j = 0; j < Iasscoiate; j++)
        {
            ICache[i].set[j].valid = ICache[i].set[j].MRU = false;
            ICache[i].set[j].tag = 0;
        }
    }

    // Report
    ITLB_hit = ITLB_miss = 0;
    IPTE_hit = IPTE_miss = 0;
    ICache_hit = ICache_miss = 0;
}

void initdMemory()
{
    // Disk
    DDisk_size = 1024;

    // PTE
    // 64 = 1024 / 16
    // DPTE_entries : 64
    DPTE_entries = DDisk_size / DPage_size;

    // TLB
    // DTLB_entries : 16

    DTLB_entries = DPTE_entries / 4;
    DTLB_index = offset((unsigned int)DPTE_entries);

    // Page offset
    DPage_offset = offset((unsigned int)DPage_size);

    // Cache Offset
    DByte_offset = 2;
    DCache_row = DCache_index = DCache_size/DBlock_size/Dasscoiate;
    DBlock_offset = offset((unsigned int)DCache_index);
    DCache_offset = DBlock_offset;

    // Memory
    DMemory_entries = DMemory_size/DPage_size;

    DMemory = new Memory[DMemory_entries];
    for(int i = 0; i < DMemory_entries; i++)
    {
        DMemory[i].valid = false;
        DMemory[i].LRU = 0;
    }

    // PTE
    DPTE = new PTE[DPTE_entries];
    for(int i = 0; i < DPTE_entries; i++)
    {
        DPTE[i].valid = false;
        DPTE[i].LRU = DPTE[i].PPN = 0;
    }

    // TLB
    DTLB = new TLB[DTLB_entries];
    for(int i = 0; i < DTLB_entries; i++)
    {
        DTLB[i].valid = false;
        DTLB[i].tag = DTLB[i].LRU = DTLB[i].PPN = 0;
    }

    // Cache
    DCache = new Cache[DCache_index];
    for(int i = 0; i < DCache_index; i++)
    {
        DCache[i].set = new Set[Dasscoiate];
        for(int j = 0; j < Dasscoiate; j++)
        {
            DCache[i].set[j].valid = DCache[i].set[j].MRU = false;
            DCache[i].set[j].tag = 0;
        }
    }


    // Report
    DTLB_hit = DTLB_miss = 0;
    DPTE_hit = DPTE_miss = 0;
    DCache_hit = DCache_miss = 0;
}

void memoryCheck(unsigned int VA)
{
    int VPN, PPN, PA;
    // VPN == Tag (Due to Fully associate)
    VPN = VA>>IPage_offset;
    // cout << "Cycle  :  " << dec << setw(2) << cyc << endl;;
    // cout << "VA : "<< setw(8) << setfill('0') << hex << VA << endl;
    PPN = iTLB_Hit(VPN);
    // cout <<"Cycle : " << dec << cyc << "--- TLB : " << dec << PPN << endl;
    // TLB Hit
    if(PPN != -1)
    {
        ITLB_hit++;
        PA = (PPN<<IPage_offset) + (VA%IPage_size);
        iCache(PA);
    }
    else
    {
        ITLB_miss++;
        PPN = iPTE(VPN);
        // cout << "PTE : "<< dec << PPN << endl;
        iTLB_Miss(VPN, PPN);
        PA = (PPN<<IPage_offset) + (VA%IPage_size);
        iCache(PA);
    }
}

unsigned int iTLB_Hit(unsigned int VPN)
{
    unsigned int tag;
    tag = VPN;
    // Hit condition
    for(int i = 0; i < ITLB_entries; i++)
    {
        if( ITLB[i].tag == tag && ITLB[i].valid == true )
        {
            ITLB[i].LRU = cyc;
            return ITLB[i].PPN;
        }
    }

    // Miss condition

    return -1;
}

void iTLB_Miss(unsigned int VPN, unsigned int PPN)
{
    // Valid bit --- false
    for(int i = 0; i < ITLB_entries; i++)
    {
        if( ITLB[i].valid == false )
        {
            ITLB[i].valid = true;
            ITLB[i].tag = VPN;
            ITLB[i].PPN = PPN;
            ITLB[i].LRU = cyc;
            return;
        }
    }
    // Valid Bit --- Whole true
    unsigned int min = ITLB[0].LRU;
    unsigned int index = 0;
    // LRU
    for(int i = 0; i < ITLB_entries; i++)
    {
        if( ITLB[i].LRU < min )
        {
            min = ITLB[i].LRU;
            index = i;
        }
    }

    ITLB[index].valid = true;
    ITLB[index].tag = VPN;
    ITLB[index].PPN = PPN;
    ITLB[index].LRU = cyc;

}

unsigned int iPTE(unsigned int VPN)
{
    // Hit condition
    for(int i = 0; i < IPTE_entries; i++)
    {
        if( IPTE[i].VPN == VPN && IPTE[i].valid == true )
        {
            IPTE_hit++;
            IPTE[i].LRU = cyc;
            return IPTE[i].PPN;
        }
    }

    // Miss condition
    // Look up LRU in Memory
    unsigned int min = IMemory[0].LRU;
    unsigned int index = 0; // PPN = index
    bool isValid = false;
    for(int i = 0; i < IMemory_entries; i++)
    {
        if( IMemory[i].valid == false)
        {
            isValid = true;
            IMemory[i].valid = true;
            IMemory[i].LRU = cyc;
            index = i;
            // cout <<"Cycle : " << dec << cyc << " --- PPN(Memory) : " << dec << index << endl;
            // cout <<"PTE(VA) : "  << setw(8) << setfill('0') << hex << PC.cur << endl;
            break;
        }
    }

    // Find the min(LRU)
    if(!isValid)
    {
        for(int i = 0; i < IMemory_entries; i++)
        {
            if( IMemory[i].LRU < min)
            {
                min = IMemory[i].LRU;
                index = i;
            }
        }
    }


    unsigned int PPN = index;
    isValid = false;

    // Valid Bit --- false
    unsigned int errVPN, errPPN;
    for(int i = 0 ; i < IPTE_entries; i++)
    {
        // Find and Break the loop
        if( IPTE[i].valid == false )
        {
            errVPN = IPTE[i].VPN;
            errPPN = IPTE[i].PPN;
            IPTE[i].VPN = VPN;
            IPTE[i].PPN = PPN;
            IPTE[i].LRU = cyc;
            IPTE[i].valid  = true;
            isValid = true;
            break;
        }
    }

    // Whole entries are occupied
    if(!isValid)
    {
        min = IPTE[0].LRU;
        index = 0;
        for(int i = 0; i < IPTE_entries; i++)
        {
            if(IPTE[i].LRU < min )
            {
                min = IPTE[i].LRU;
                index = i;
            }
        }
        errVPN = IPTE[index].VPN;
        errPPN = IPTE[index].PPN;
        IPTE[index].VPN = VPN;
        IPTE[index].PPN = PPN;
        IPTE[index].LRU = cyc;
        IPTE[index].valid  = true;

        // Turn into invalid(PTE)
        for(int i = 0; i < IPTE_entries; i++)
            if( IPTE[i].PPN == PPN && IPTE[i].valid==true && IPTE[i].VPN != VPN) // Check Page Fault
                IPTE[i].valid = false;

        // Turn into invalid(TLB)
        for(int i = 0; i < ITLB_entries; i++)
            if( ITLB[i].PPN == PPN && ITLB[i].valid==true && ITLB[i].tag != VPN)
                ITLB[i].valid = false;

        // Turn into invalid(Cache)
        for(int i = 0; i < IPage_size; i++)
        {
            unsigned int PA;
            unsigned int cacheTag, cacheIndex;
            PA = (errPPN<<IPage_offset) + i;
            cacheIndex = (PA>>ICache_offset)%ICache_index;
            cacheTag   = (PA>>ICache_offset)/ICache_index;
            for(int j = 0; j < Iasscoiate; j++)
                if( ICache[cacheIndex].set[j].valid == true &&ICache[cacheIndex].set[j].tag == cacheTag )
                    ICache[cacheIndex].set[j].valid = false;
        }
    }

    IPTE_miss++;
    return PPN;
}

void iCache(unsigned int PA)
{
    // cout << "PA : " << setw(8) << setfill('0') << hex << PA << endl;
    unsigned int cacheTag, cacheIndex;
    // cout << "Cycle : " << dec << setw(2) << cyc << endl;
    cacheIndex = (PA>>ICache_offset)%ICache_index;
    // cout << "cacheIndex : " << dec << setw(2) << cacheIndex << endl;
    cacheTag   = (PA>>ICache_offset)/ICache_index;
    // cout << "cacheTag : " << hex << setw(8) << setfill('0') << cacheTag << endl;

    // Hit condition
    for(int i = 0; i < Iasscoiate; i++)
    {
        if(ICache[cacheIndex].set[i].tag == cacheTag && ICache[cacheIndex].set[i].valid == true)
        {
            // cout << "Hit Cycle : " << setw(2) << dec << cyc << endl;
            ICache[cacheIndex].set[i].MRU = true;
            ICache_hit++;
            return;
        }
    }

    // Miss condition
    // Valid Bit false condition
    bool isValid = false;
    for(int i = 0; i < Iasscoiate; i++)
    {
        if(ICache[cacheIndex].set[i].valid == false)
        {
            // cout << "#(Set-Valid) : " << dec << i << endl;
            ICache[cacheIndex].set[i].MRU = true;
            ICache[cacheIndex].set[i].valid = true;
            ICache[cacheIndex].set[i].tag = cacheTag;
            isValid = true;
            // Check MRU
            unsigned int temp = 0;
            for(int j = 0; j < Iasscoiate; j++)
                if(ICache[cacheIndex].set[j].MRU == true)
                    temp++;
            if(temp == Iasscoiate)
                for(int j = 0; j < Iasscoiate; j++)
                    ICache[cacheIndex].set[j].MRU = false;
            ICache[cacheIndex].set[i].MRU = true;
            break;
        }
    }

    // Valid Bit --- Whole true in one set
    if(!isValid)
    {
        if(Iasscoiate==1)
        {
            ICache[cacheIndex].set[0].MRU = true;
            ICache[cacheIndex].set[0].valid = true;
            ICache[cacheIndex].set[0].tag = cacheTag;
        }
        else
        {
            for(int i = 0; i < Iasscoiate; i++)
            {
                if(ICache[cacheIndex].set[i].MRU == false)
                {
                    // cout << "#(Set-MRU) : " << dec << i << endl;
                    ICache[cacheIndex].set[i].MRU = true;
                    ICache[cacheIndex].set[i].valid = true;
                    ICache[cacheIndex].set[i].tag = cacheTag;
                    // Check MRU
                    unsigned int temp = 0;
                    for(int j = 0; j < Iasscoiate; j++)
                        if(ICache[cacheIndex].set[j].MRU == true)
                            temp++;
                    if(temp == Iasscoiate)
                        for(int j = 0; j < Iasscoiate; j++)
                            ICache[cacheIndex].set[j].MRU = false;
                    ICache[cacheIndex].set[i].MRU = true;
                    break;
                }
            }
        }
    }

    ICache_miss++;
    return;


}

void iReport()
{
    // Cache
    report << "ICache :\n";
    report << "# hits: " << dec << ICache_hit << "\n";
    report << "# misses: " << dec << ICache_miss << "\n";
    report << "\n";
    report << "DCache :\n";
    report << "# hits: " << dec << DCache_hit << "\n";
    report << "# misses: " << dec << DCache_miss << "\n";
    report << "\n";
    // TLB
    report << "ITLB :\n";
    report << "# hits: " << dec << ITLB_hit << "\n";
    report << "# misses: " << dec << ITLB_miss << "\n";
    report << "\n";
    report << "DTLB :\n";
    report << "# hits: " << dec << DTLB_hit << "\n";
    report << "# misses: " << dec << DTLB_miss << "\n";
    report << "\n";
    // PTE
    report << "IPageTable :\n";
    report << "# hits: " << dec << IPTE_hit << "\n";
    report << "# misses: " << dec << IPTE_miss << "\n";
    report << "\n";
    report << "DPageTable :\n";
    report << "# hits: " << dec << DPTE_hit << "\n";
    report << "# misses: " << dec << DPTE_miss << "\n";
    report << "\n";
}
