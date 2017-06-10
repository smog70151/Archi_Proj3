#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cstdlib>
// to include the var
#include "var.h"
// to detect the error
#include "error_detect.h"

using namespace std;

void dProcessor(unsigned int VA);
unsigned int dTLB_Hit(unsigned int VPN); // Return PPN
void dTLB_Miss(unsigned int VPN, unsigned int PPN); // Write form TLB
unsigned int dPTE(unsigned int VPN);  // Return PPN for TLB
void dCache(unsigned int PA);
bool isWrite;

/* R-Type Instructions */ //17
//No shamt
void R_add();
void R_addu();
void R_sub();
void R_and();
void R_or();
void R_xor();
void R_nor();
void R_nand();
void R_slt();
//No rs
void R_sll();
void R_srl();
void R_sra();
//Only rs
void R_jr();
//rs and rt
void R_mult();
void R_multu();
//Only rd
void R_mfhi();
void R_mflo();
/* R-Type Instructions */

/* I-Type Instructions */ //18
void I_addi();
void I_addiu();
void I_lw();
void I_lh();
void I_lhu();
void I_lb();
void I_lbu();
void I_sw();
void I_sh();
void I_sb();
void I_lui();
void I_andi();
void I_ori();
void I_nori();
void I_slti();
void I_beq();
void I_bne();
void I_bgtz();
/* I-Type Instructions */

/* J-Type Instructions */
void J_j();
void J_jal();
/* J-Type Instructions */

/* S-Type Instructions */
void halt();
/* S-Type Instructions */

//implement the meaning of the instruction
void ALU()
{
    switch(opcode)
    {
    case 0x00 :
        switch(funct)
        {
        case 0x20 : //add $d,$s,$t
            R_add();
        break;
        case 0x21 : //addu $d,$s,$t
            R_addu();
        break;
        case 0x22 : //sub $d,$s,$t
            R_sub();
        break;
        case 0x24 : //and $d,$s,$t
            R_and();
        break;
        case 0x25 : //or $d,$s,$t
            R_or();
        break;
        case 0x26 : //xor $d,$s,$t
            R_xor();
        break;
        case 0x27 : //nor $d,$s,$t
            R_nor();
        break;
        case 0x28 : //nand $d,$s,$t
            R_nand();
        break;
        case 0x2a : //slt $d,$s,$t
            R_slt();
        break;
        case 0x00 : //sll $d,$t,C
            R_sll();
        break;
        case 0x02 : //srl $d,$t,C
            R_srl();
        break;
        case 0x03 : //sra $d,$t,C
            R_sra();
        break;
        case 0x08 : //jr $s
            R_jr();
        break;
        case 0x18 : //mult $s $t
            R_mult();
        break;
        case 0x19 : //multu $s $t
            R_multu();
        break;
        case 0x10 : //mfhi $d
            R_mfhi();
        break;
        case 0x12 : //mflo $d
            R_mflo();
        break;
        default :
            cout << "illegal instruction found at 0xaddress" ;
            error_halt = 1;
        break;
        }
    break;
    case 0x08 : //addi $t,$s,C
        I_addi();
    break;
    case 0x09 : //addiu $t,$s,C
        I_addiu();
    break;
    case 0x23 : //lw $t,C($s)
        I_lw();
    break;
    case 0x21 : //lh $t,C($s)
        I_lh();
    break;
    case 0x25 : //lhu $t,C($s)
        I_lhu();
    break;
    case 0x20 : //lb $t,C($s)
        I_lb();
    break;
    case 0x24 : //lbu $t,C($s)
        I_lbu();
    break;
    case 0x2b : //sw $t,C($s)
        I_sw();
    break;
    case 0x29 : //sh $t,C($s)
        I_sh();
    break;
    case 0x28 : //sb $t,C($s)
        I_sb();
    break;
    case 0x0f : //lui $t,C
        I_lui();
    break;
    case 0x0c : //andi $t,C
        I_andi();
    break;
    case 0x0d : //ori $t,C
        I_ori();
    break;
    case 0x0e : //nori $t,C
        I_nori();
    break;
    case 0x0a : //slti $t,C
        I_slti();
    break;
    case 0x04 : //beq $s,$t,C
        I_beq();
    break;
    case 0x05 : //bne $s,$t,C
        I_bne();
    break;
    case 0x07 : //bgtz $s,$t,C
        I_bgtz();
    break;
    case 0x02 : //j C
        J_j();
    break;
    case 0x03 : //jal C
        J_jal();
    break;
    case 0x3f : //halt
        halt();
    break;
    default :
        cout << "illegal instruction found at 0xaddress" ;
        error_halt = 1;
    break;
    }
}

/* R-Type instructions */
/* No shamt */
void R_add()
{
    //$d = $s + $t
    reg[rd].cur = read_data1 + read_data2;
    Error_R0(); //detect Write Register[0]
    Error_OVF(); //detect Adder OVF
}
void R_addu()
{
    //$d = $s + $t(unsigned, no overflow exception)
    reg[rd].cur = read_data1 + read_data2;
    Error_R0(); //detect Write Register[0]
}
void R_sub()
{
    //$d = $s - $t
    reg[rd].cur = read_data1 - read_data2;
    Error_R0(); //detect Write Register[0]
    Error_OVF(); //detect Adder OVF
}
void R_and()
{
    //$d = $s & $t
    reg[rd].cur = read_data1 & read_data2;
    Error_R0(); //detect Write Register[0]
}
void R_or()
{
    //$d = $s | $t
    reg[rd].cur = read_data1 | read_data2;
    Error_R0(); //detect Write Register[0]
}
void R_xor()
{
    //$d = $s ^ $t
    reg[rd].cur = read_data1 ^ read_data2;
    Error_R0(); //detect Write Register[0]
}
void R_nor()
{
    //$d = ~ ($s | $t)
    reg[rd].cur = ~(read_data1 | read_data2);
    Error_R0(); //detect Write Register[0]
}
void R_nand()
{
    //$d = ~($s & $t)
    reg[rd].cur = ~(read_data1 & read_data2);
    Error_R0(); //detect Write Register[0]
}
void R_slt()
{
    //$d = ($s < $t), signed comparison
    reg[rd].cur = read_data1 < read_data2;
    Error_R0(); //detect Write Register[0]
}

/* No rs */
void R_sll()
{
    //$d = $t << C
    reg[rd].cur = read_data2 << shamt;
    if( rt != 0 || rd !=0 || shamt !=0 )
        Error_R0(); //detect Write Register[0]
}
void R_srl()
{
    //$d = $t >> C
    reg[rd].cur = (unsigned int)read_data2 >> shamt;

    Error_R0(); //detect Write Register[0]
}
void R_sra()
{
    //$d = $t >> C, with sign bit shifted in
    if(read_data2 & 0x80000000)
        reg[rd].cur = (0xffffffff00000000 | (unsigned long long ) read_data2) >> shamt;
    else
        reg[rd].cur = read_data2 >> shamt;
    Error_R0(); //detect Write Register[0]
}

/* Only rs */
void R_jr()
{
    //PC=$s
    PC.cur = read_data1;
}

/* rs and rt */
void R_mult()
{
    //{Hi || Lo} = $s * $t
    unsigned long long temp_rs, temp_rt;
    /* Extension signed rs, rt */
    if(read_data1 &0x80000000)
        temp_rs = read_data1 | 0xffffffff00000000;
    else
        temp_rs = read_data1 & 0x00000000ffffffff;
    if(read_data2 &0x80000000)
        temp_rt = read_data2 | 0xffffffff00000000;
    else
        temp_rt = read_data2 & 0x00000000ffffffff;
    HI.cur = (temp_rs*temp_rt) >> 32;
    LO.cur = (temp_rs*temp_rt) & 0x00000000ffffffff;
    Error_OVW(); //detect OVW HI, LO
}
void R_multu()
{
    //{Hi || Lo} = $s * $t (unsigned, no overflow exception)
    unsigned long long temp_rs, temp_rt;
    /* Extension rs, rt */
    temp_rs = read_data1 & 0x00000000ffffffff;
    temp_rt = read_data2 & 0x00000000ffffffff;

    HI.cur = (temp_rs*temp_rt) >> 32;
    LO.cur = (temp_rs*temp_rt) & 0x00000000ffffffff;
    Error_OVW(); //detect OVW HI, LO
}

/* Only rd */
void R_mfhi()
{
    //$d = Hi
    reg[rd].cur = HI.cur;
    Error_R0(); //detect Write Register[0]
    Flag_OVW(); //Flag to detect OVW
}
void R_mflo()
{
    //$d = Lo
    reg[rd].cur = LO.cur;
    Error_R0(); //detect Write Register[0]
    Flag_OVW(); //Flag to detect OVW
}

/* R-Type instructions */

/* I-Type Instructions */ //18
void I_addi()
{
    //$t = $s + C(signed)
    reg[rt].cur = read_data1 + simmediate;
    Error_R0(); //detect Write Register[0]
    Error_OVF(); //detect Adder OVF
}
void I_addiu()
{
    //$t = $s + C(unsigned, no overflow exception)
    reg[rt].cur = read_data1 + simmediate;
    Error_R0(); //detect Write Register[0]
}
void I_lw()
{
    //$t = 4 bytes from Memory[$s + C(signed)]
    int addr;
    addr = read_data1 + simmediate;
    // isWrite = false;
    dProcessor((unsigned int)addr);
    if(0<=addr&&addr<=1020)
        reg[rt].cur = (data_mem[addr  ] << 24)
                    + (data_mem[addr+1] << 16)
                    + (data_mem[addr+2] <<  8)
                    + (data_mem[addr+3]      );
    Error_R0(); //detect Write Register[0]
    Error_OVF(); //detect Adder OVF
    Error_Dmem(addr); //detect D Memory OVF
    Error_Misaligned(addr); //detect S/L Wrong Memory

}
void I_lh()
{
    //$t = 2 bytes from Memory[$s + C(signed)], signed
    int addr;
    addr = read_data1 + simmediate;
    // isWrite = false;
    dProcessor((unsigned int)addr);
    if(0<=addr&&addr<=1022)
    {
        reg[rt].cur = (data_mem[addr  ] << 8)
                    + (data_mem[addr+1]     );
        if( reg[rt].cur & 0x00008000 )
            reg[rt].cur = reg[rt].cur | 0xffff0000;
    }
    Error_R0(); //detect Write Register[0]
    Error_OVF(); //detect Adder OVF
    Error_Dmem(addr); //detect D Memory OVF
    Error_Misaligned(addr); //detect S/L Wrong Memory
}
void I_lhu()
{
    //$t = 2 bytes from Memory[$s + C(signed)], unsigned
    int addr;
    addr = read_data1 + simmediate;
    // isWrite = false;
    dProcessor((unsigned int)addr);
    if(0<=addr&&addr<=1022)
        reg[rt].cur = (data_mem[addr  ] << 8)
                    + (data_mem[addr+1]     );
    Error_R0(); //detect Write Register[0]
    Error_OVF(); //detect Adder OVF
    Error_Dmem(addr); //detect D Memory OVF
    Error_Misaligned(addr); //detect S/L Wrong Memory
}
void I_lb()
{
    //$t = Memory[$s + C(signed)], signed
    int addr;
    addr = read_data1 + simmediate;
    // isWrite = false;
    dProcessor((unsigned int)addr);
    if(0<=addr&&addr<=1023)
    {
        reg[rt].cur = data_mem[addr];
        if( reg[rt].cur & 0x00000080 )
            reg[rt].cur = reg[rt].cur | 0xffffff00;
    }
    Error_R0(); //detect Write Register[0]
    Error_OVF(); //detect Adder OVF
    Error_Dmem(addr); //detect D Memory OVF
    Error_Misaligned(addr); //detect S/L Wrong Memory
}
void I_lbu()
{
    //$t = Memory[$s + C(signed)], unsigned
    int addr;
    addr = read_data1 + simmediate;
    // isWrite = false;
    dProcessor((unsigned int)addr);
    if(0<=addr&&addr<=1023)
        reg[rt].cur = data_mem[addr];
    Error_R0(); //detect Write Register[0]
    Error_OVF(); //detect Adder OVF
    Error_Dmem(addr); //detect D Memory OVF
    Error_Misaligned(addr); //detect S/L Wrong Memory
}
void I_sw()
{
    //4 bytes from Memory[$s + C(signed)] = $t
    int addr;
    addr = read_data1 + simmediate;
    // isWrite = true;
    dProcessor((unsigned int)addr);
    if(0<=addr&&addr<=1020)
    {
        data_mem[addr  ] = (reg[rt].cur >> 24) & 0x000000ff;
        data_mem[addr+1] = (reg[rt].cur >> 16) & 0x000000ff;
        data_mem[addr+2] = (reg[rt].cur >>  8) & 0x000000ff;
        data_mem[addr+3] = (reg[rt].cur      ) & 0x000000ff;
    }
    Error_OVF(); //detect Adder OVF
    Error_Dmem(addr); //detect D Memory OVF
    Error_Misaligned(addr); //detect S/L Wrong Memory
}
void I_sh()
{
    //2 bytes from Memory[$s + C(signed)] = $t & 0x0000FFFF
    int addr;
    addr = read_data1 + simmediate;
    // isWrite = true;
    dProcessor((unsigned int)addr);
    if(0<=addr&&addr<=1022)
    {
        data_mem[addr  ] = (reg[rt].cur >>  8) & 0x000000ff;
        data_mem[addr+1] = (reg[rt].cur      ) & 0x000000ff;
    }
    Error_OVF(); //detect Adder OVF
    Error_Dmem(addr); //detect D Memory OVF
    Error_Misaligned(addr); //detect S/L Wrong Memory
}
void I_sb()
{
    //Memory[$s + C(signed)] = $t & 0x000000FF
    int addr;
    addr = read_data1 + simmediate;
    isWrite = true;
    dProcessor((unsigned int)addr);
    if(0<=addr&&addr<=1023)
        data_mem[addr  ] = (reg[rt].cur      ) & 0x000000ff;
    Error_OVF(); //detect Adder OVF
    Error_Dmem(addr); //detect D Memory OVF
    Error_Misaligned(addr); //detect S/L Wrong Memory
}
void I_lui()
{
    //$t = C << 16
    reg[rt].cur = immediate << 16;
    Error_R0(); //detect Write Register[0]
}
void I_andi()
{
    //$t = $s & C(unsigned)
    reg[rt].cur = read_data1 & immediate;
    Error_R0(); //detect Write Register[0]
}
void I_ori()
{
    //$t = $s | C(unsigned)
    reg[rt].cur = read_data1 | immediate;
    Error_R0(); //detect Write Register[0]
}
void I_nori()
{
    //$t = ~($s | C(unsigned))
    reg[rt].cur = ~(read_data1 | immediate);
    Error_R0(); //detect Write Register[0]
}
void I_slti()
{
    //$t = ($s < C(signed) ), signed comparison
    reg[rt].cur = read_data1 < simmediate;
    Error_R0(); //detect Write Register[0]
}
void I_beq()
{
    //if ($s == $t) go to PC+4+4*C(signed)
    if( read_data1 == read_data2 )
    {
        PC.cur = PC.cur + ( simmediate << 2 );
    }
    Error_OVF(); //detect Adder OVF
}
void I_bne()
{
    //if ($s != $t) go to PC+4+4*C(signed)
    if( read_data1 != read_data2 )
    {
        PC.cur = PC.cur + ( simmediate << 2 );
    }
    Error_OVF(); //detect Adder OVF
}
void I_bgtz()
{
    //if ($s > 0) go to PC+4+4*C(signed)
    if( read_data1 > 0)
    {
        PC.cur = PC.cur + ( simmediate << 2 );
    }
    Error_OVF(); //detect Adder OVF
}
/* I-Type Instructions */

/* J-Type Instructions */
void J_j()
{
    PC.cur = ( PC.cur >> 27 ) | ( immediate << 2 );
}
void J_jal()
{
    reg[31].cur = PC.cur;
    PC.cur = ( PC.cur >> 27 ) | ( immediate << 2 );
}
/* J-Type Instructions */

/* S-Type Instructions */
void halt()
{
    error_halt = 1;
}
/* S-Type Instructions */

void dProcessor(unsigned int VA)
{
    int VPN, PPN, PA;
    // VPN == Tag (Due to Fully associate)
    VPN = VA>>DPage_offset;
    // cout << "Cycle  :  " << dec << setw(2) << cyc << endl;;
    // cout << "VA : "<< setw(8) << setfill('0') << hex << VA << endl;
    PPN = dTLB_Hit(VPN);
    // cout <<"Cycle : " << dec << cyc << "--- TLB : " << dec << PPN << endl;
    // TLB Hit
    bool isTLBHit = false;
    if(PPN>=0&&PPN<DMemory_entries)
        isTLBHit = true;
    if(isTLBHit)
    {
        PA = (PPN<<DPage_offset) + (VA%DPage_size);
        dCache(PA);
        // cout << "(DTLB)Hit Cycle : " << setw(2) << dec << cyc << endl;
        // if(isWrite)
        // PPN = dPTE(VPN);
    }
    else
    {
        PPN = dPTE(VPN);
        // cout << "PTE : "<< dec << PPN << endl;
        dTLB_Miss(VPN, PPN);
        PA = (PPN<<DPage_offset) + (VA%DPage_size);
        dCache(PA);
    }
}

unsigned int dTLB_Hit(unsigned int VPN) // Return PPN
{
    unsigned int tag;
    tag = VPN; // TLB Fully associate => tag = VPN
    // Hit condition
    for(int i = 0; i < DTLB_entries; i++)
    {
        if( DTLB[i].tag == tag && DTLB[i].valid == true )
        {
            DTLB_hit++;
            DTLB[i].LRU = cyc;
            // Write Word
            // if ( isWrite ) DTLB[i].valid = false;
            return DTLB[i].PPN;
        }
    }

    // Miss condition
    DTLB_miss++;
    return -1;
}

void dTLB_Miss(unsigned int VPN, unsigned int PPN) // Write form TLB
{
    // Valid bit --- false
    for(int i = 0; i < DTLB_entries; i++)
    {
        if( DTLB[i].valid == false )
        {
            DTLB[i].valid = true;
            DTLB[i].tag = VPN;
            DTLB[i].PPN = PPN;
            DTLB[i].LRU = cyc;
            // Write word
            // if(isWrite) DTLB[i].valid = false;
            return;
        }
    }
    // Valid Bit --- Whole true
    unsigned int min = DTLB[0].LRU;
    unsigned int index = 0;
    // LRU
    for(int i = 0; i < DTLB_entries; i++)
    {
        if( DTLB[i].LRU < min )
        {
            min = DTLB[i].LRU;
            index = i;
        }
    }

    DTLB[index].valid = true;
    DTLB[index].tag = VPN;
    DTLB[index].PPN = PPN;
    DTLB[index].LRU = cyc;
    // Write word
    // if(isWrite) DTLB[index].valid = false;

}

unsigned int dPTE(unsigned int VPN)  // Return PPN for TLB
{
    bool isPageFault = false;
    // Hit condition
    for(int i = 0; i < DPTE_entries; i++)
    {
        if( DPTE[i].VPN == VPN && DPTE[i].valid == true )
        {
            DPTE_hit++;
            // cout << "(DPTE)Hit Cycle : " << setw(2) << dec << cyc << endl;
            DPTE[i].LRU = cyc;
            DMemory[DPTE[i].PPN].LRU = cyc;
            // Write word
            // if(isWrite) DPTE[i].valid = false;
            return DPTE[i].PPN;
        }
    }

    // Miss condition
    // Look up LRU in Memory
    unsigned int min = DMemory[0].LRU;
    unsigned int index = 0; // PPN = index
    bool isValid = false;
    for(int i = 0; i < DMemory_entries; i++)
    {
        if( DMemory[i].valid == false)
        {
            isValid = true;
            DMemory[i].valid = true;
            DMemory[i].LRU = cyc;
            index = i;
            // Write word
            // if(isWrite) DTLB[index].valid = false;
            // cout <<"Cycle : " << dec << cyc << " --- PPN(Memory) : " << dec << index << endl;
            // cout <<"PTE(VA) : "  << setw(8) << setfill('0') << hex << PC.cur << endl;
            break;
        }
    }

    // Find the min(LRU)
    if(!isValid)
    {
        for(int i = 0; i < DMemory_entries; i++)
        {
            if( DMemory[i].LRU < min)
            {
                min = DMemory[i].LRU;
                index = i;
            }
        }
        isPageFault = true;
        DMemory[index].valid = true;
        DMemory[index].LRU = cyc;
    }


    unsigned int PPN = index;
    if(isPageFault)
    {
        // Turn into invalid(PTE)
        for(int i = 0; i < DPTE_entries; i++)
            if( DPTE[i].PPN == PPN && DPTE[i].valid==true ) // Check Page Fault
                DPTE[i].valid = false;

        // Turn into invalid(TLB)
        for(int i = 0; i < DTLB_entries; i++)
            if( DTLB[i].PPN == PPN && DTLB[i].valid==true )
                DTLB[i].valid = false;

        // Turn into invalid(Cache)
        for(int i = 0; i < DPage_size; i++)
        {
            unsigned int PA;
            unsigned int cacheTag, cacheIndex;
            PA = (PPN<<DPage_offset) + i;
            cacheIndex = (PA>>DCache_offset)%DCache_index;
            cacheTag   = (PA>>DCache_offset)/DCache_index;
            for(int j = 0; j < Dasscoiate; j++)
            {
                if( DCache[cacheIndex].set[j].tag == cacheTag )
                {
                    DCache[cacheIndex].set[j].valid = false;
                    DCache[cacheIndex].set[j].MRU = false;
                }
            }
        }
    }

    isValid = false;

    // Valid Bit --- false
    // unsigned int errVPN, errPPN;
    for(int i = 0 ; i < DPTE_entries; i++)
    {
        // Find and Break the loop
        if( DPTE[i].valid == false )
        {
            // errVPN = DPTE[i].VPN;
            // errPPN = DPTE[i].PPN;
            DPTE[i].VPN = VPN;
            DPTE[i].PPN = PPN;
            DPTE[i].LRU = cyc;
            DPTE[i].valid  = true;
            isValid = true;
            // Write word
            // if(isWrite) DTLB[index].valid = false;
            break;
        }
    }

    // Whole entries are occupied
    if(!isValid)
    {
        min = DPTE[0].LRU;
        index = 0;
        for(int i = 0; i < DPTE_entries; i++)
        {
            if(DPTE[i].LRU < min )
            {
                min = DPTE[i].LRU;
                index = i;
            }
        }
        // errVPN = DPTE[index].VPN;
        // errPPN = DPTE[index].PPN;
        DPTE[index].VPN = VPN;
        DPTE[index].PPN = PPN;
        DPTE[index].LRU = cyc;
        DPTE[index].valid  = true;
        // Write word
        // if(isWrite) DTLB[index].valid = false;


    }

    DPTE_miss++;
    return PPN;
}

void dCache(unsigned int PA)
{
    // trace << "(D)Cycle : " << dec << setw(2) << cyc << endl;
    // trace << "PA : " << setw(8) << setfill('0') << hex << PA << endl;
    unsigned int cacheTag, cacheIndex;
    cacheIndex = (PA>>DCache_offset)%DCache_index;
    // trace << "cacheIndex : " << dec << setw(2) << cacheIndex << endl;
    cacheTag   = (PA>>DCache_offset)/DCache_index;
    // trace << "cacheTag : " << hex << setw(8) << setfill('0') << cacheTag << endl;

    // Hit condition
    for(int i = 0; i < Dasscoiate; i++)
    {
        if(DCache[cacheIndex].set[i].tag == cacheTag && DCache[cacheIndex].set[i].valid == true)
        {
            // trace << "(D)Hit Cycle : " << setw(2) << dec << cyc << endl << endl;
            DCache[cacheIndex].set[i].MRU = true;
            DCache_hit++;
            unsigned int temp = 0;
            for(int j = 0; j < Dasscoiate; j++)
                if(DCache[cacheIndex].set[j].MRU == true)
                    temp++;
            if(temp == Dasscoiate)
                for(int j = 0; j < Dasscoiate; j++)
                    DCache[cacheIndex].set[j].MRU = false;
            DCache[cacheIndex].set[i].MRU = true;
            // Write word
            // if(isWrite) DCache[cacheIndex].set[i].valid = false;
            return;
        }
    }
    // trace << endl;

    // Miss condition
    // Valid Bit false condition
    bool isValid = false;
    for(int i = 0; i < Dasscoiate; i++)
    {
        if(DCache[cacheIndex].set[i].valid == false)
        {
            // cout << "(D)#(Valid Miss Cycle) : " << dec << cyc << endl;
            DCache[cacheIndex].set[i].valid = true;
            DCache[cacheIndex].set[i].MRU = true;
            DCache[cacheIndex].set[i].tag = cacheTag;
            isValid = true;
            // Write word
            // if(isWrite) DCache[cacheIndex].set[i].valid = false;
            // Check MRU
            unsigned int temp = 0;
            for(int j = 0; j < Dasscoiate; j++)
                if(DCache[cacheIndex].set[j].MRU == true)
                    temp++;
            if(temp == Dasscoiate)
                for(int j = 0; j < Dasscoiate; j++)
                    DCache[cacheIndex].set[j].MRU = false;
            DCache[cacheIndex].set[i].MRU = true;
            break;
        }
    }

    // Valid Bit --- Whole true in one set
    if(!isValid)
    {
        if(Dasscoiate==1)
        {
            DCache[cacheIndex].set[0].MRU = true;
            DCache[cacheIndex].set[0].valid = true;
            DCache[cacheIndex].set[0].tag = cacheTag;
        }
        else
        {
            for(int i = 0; i < Dasscoiate; i++)
            {
                if(DCache[cacheIndex].set[i].MRU == false)
                {
                    // cout << "(D)#(MRU Miss Cycle) : " << dec << cyc << endl;
                    DCache[cacheIndex].set[i].MRU = true;
                    DCache[cacheIndex].set[i].valid = true;
                    DCache[cacheIndex].set[i].tag = cacheTag;
                    // Write word
                    // if(isWrite) DCache[cacheIndex].set[i].valid = false;
                    // Check MRU
                    unsigned int temp = 0;
                    for(int j = 0; j < Dasscoiate; j++)
                        if(DCache[cacheIndex].set[j].MRU == true)
                            temp++;
                    if(temp == Dasscoiate)
                        for(int j = 0; j < Dasscoiate; j++)
                            DCache[cacheIndex].set[j].MRU = false;
                    DCache[cacheIndex].set[i].MRU = true;
                    break;
                }
            }
        }
    }

    DCache_miss++;
    return;
}
