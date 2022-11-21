#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>
using namespace std;
#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;
};

struct EXStruct {
    bitset<64>  Read_data1;
    bitset<64>  Read_data2;
    bitset<64>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem;
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu
    bool        wrt_enable;
    bool        nop;
};

struct MEMStruct {
    bitset<64>  ALUresult;
    bitset<64>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem;
    bool        wrt_enable;
    bool        nop;
};

struct WBStruct {
    bitset<64>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class RF
{
public:
    bitset<64> Reg_data;
    RF()
    {
        Registers.resize(32);
        Registers[0] = bitset<64> (0);
    }

    bitset<64> readRF(bitset<5> Reg_addr)
    {
        Reg_data = Registers[Reg_addr.to_ulong()];
        return Reg_data;
    }

    void writeRF(bitset<5> Reg_addr, bitset<64> Wrt_reg_data)
    {
        Registers[Reg_addr.to_ulong()] = Wrt_reg_data;
    }

    void outputRF()
    {
        ofstream rfout;
        rfout.open("RFresult.txt",std::ios_base::app);
        if (rfout.is_open())
        {
            rfout<<"State of RF:\t"<<endl;
            for (int j = 0; j<32; j++)
            {
                rfout << Registers[j]<<endl;
            }
        }
        else cout<<"Unable to open file";
        rfout.close();
    }

private:
    vector<bitset<64> >Registers;
};

class INSMem
{
public:
    bitset<32> Instruction;
    INSMem()
    {
        IMem.resize(MemSize);
        ifstream imem;
        string line;
        int i=0;
//      imem.open("imem-controllHazard.txt");
//		imem.open("imem-load-useHazard.txt");
      imem.open("imem-simpleDataHazard.txt");
        if (imem.is_open())
        {
            while (getline(imem,line))
            {
                IMem[i] = bitset<8>(line);
                i++;
            }
        }
        else cout<<"Unable to open file";
        imem.close();
    }

    bitset<32> readInstr(bitset<32> ReadAddress)
    {
        string insmem;
        insmem.append(IMem[ReadAddress.to_ulong()].to_string());
        insmem.append(IMem[ReadAddress.to_ulong()+1].to_string());
        insmem.append(IMem[ReadAddress.to_ulong()+2].to_string());
        insmem.append(IMem[ReadAddress.to_ulong()+3].to_string());
        Instruction = bitset<32>(insmem);		//read instruction memory
        return Instruction;
    }

private:
    vector<bitset<8> > IMem;
};

class DataMem
{
public:
    bitset<64> ReadData;
    DataMem()
    {
        DMem.resize(MemSize);
        ifstream dmem;
        string line;
        int i=0;
      dmem.open("dmem-simpleData.txt");
//        dmem.open("dmem-load-use&controll.txt");
        if (dmem.is_open())
        {
            while (getline(dmem,line))
            {
                DMem[i] = bitset<8>(line);
                i++;
            }
        }
        else cout<<"Unable to open file";
        dmem.close();
    }

    bitset<64> readDataMem(bitset<32> Address)
    {
        string datamem;
        datamem.append(DMem[Address.to_ulong()].to_string());
        datamem.append(DMem[Address.to_ulong()+1].to_string());
        datamem.append(DMem[Address.to_ulong()+2].to_string());
        datamem.append(DMem[Address.to_ulong()+3].to_string());
        datamem.append(DMem[Address.to_ulong()+4].to_string());
        datamem.append(DMem[Address.to_ulong()+5].to_string());
        datamem.append(DMem[Address.to_ulong()+6].to_string());
        datamem.append(DMem[Address.to_ulong()+7].to_string());
        ReadData = bitset<64>(datamem);		//read data memory
        return ReadData;
    }

    void writeDataMem(bitset<32> Address, bitset<64> WriteData)
    {
        DMem[Address.to_ulong()] = bitset<8>(WriteData.to_string().substr(0,8));
        DMem[Address.to_ulong()+1] = bitset<8>(WriteData.to_string().substr(8,8));
        DMem[Address.to_ulong()+2] = bitset<8>(WriteData.to_string().substr(16,8));
        DMem[Address.to_ulong()+3] = bitset<8>(WriteData.to_string().substr(24,8));
        DMem[Address.to_ulong()+4] = bitset<8>(WriteData.to_string().substr(32,8));
        DMem[Address.to_ulong()+5] = bitset<8>(WriteData.to_string().substr(40,8));
        DMem[Address.to_ulong()+6] = bitset<8>(WriteData.to_string().substr(48,8));
        DMem[Address.to_ulong()+7] = bitset<8>(WriteData.to_string().substr(56,8));
    }

    void outputDataMem()
    {
        ofstream dmemout;
        dmemout.open("dmemresult.txt");
        if (dmemout.is_open())
        {
            for (int j = 0; j< 1000; j++)
            {
                dmemout << DMem[j]<<endl;
            }

        }
        else cout<<"Unable to open file";
        dmemout.close();
    }

private:
    vector<bitset<8> > DMem;
};

void printState(stateStruct state, int cycle)
{
    ofstream printstate;
    printstate.open("stateresult.txt", std::ios_base::app);
    if (printstate.is_open())
    {
        printstate<<"State after executing cycle:\t"<<cycle<<endl;

        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;
        printstate<<"IF.nop:\t"<<state.IF.nop<<endl;

        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl;
        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;

        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl;
        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl;
        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;
        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;

        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl;
        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;
        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;
        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl;
        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;
        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;

        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;
        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;
        printstate<<"WB.nop:\t"<<state.WB.nop<<endl;
    }
    else cout<<"Unable to open file";
    printstate.close();
}


int main()
{

    RF myRF;
    INSMem myInsMem;
    DataMem myDataMem;
    struct stateStruct state{0};
    state.IF.nop = false;
    state.ID.nop = true;
    state.EX.nop = true;
    state.MEM.nop = true;
    state.WB.nop = true;
    state.EX.alu_op = true;
    int cycle = 0;
    int lu_flag=0;  // 是否有load-use冒险，若有，则PC保持；若无，则PC+4

    while (1) {
//        int lu_flag=0;
//        int flag=0;

        /* --------------------- WB stage --------------------- */
        if(!state.WB.nop){
            if(state.WB.wrt_enable){
                cout<<"writeRF:"<<state.WB.Wrt_reg_addr.to_ulong()<<' '<<state.WB.Wrt_data.to_ulong()<<endl;
                myRF.writeRF(state.WB.Wrt_reg_addr,state.WB.Wrt_data);
            }
        }
        state.WB.nop = state.MEM.nop;

        /* --------------------- MEM stage --------------------- */
        if(!state.MEM.nop)
        {
            // ld:rd<-(rs1+offset)
            // 64位数据->32位地址
            bitset<32> tmpALUResult=bitset<32>(state.MEM.ALUresult.to_string().substr(32,32));
            if(state.MEM.rd_mem)
            {
                state.WB.Wrt_data=myDataMem.readDataMem(tmpALUResult);
            }
                // R:rd<-(rs1+rs2/imm)
            else
                state.WB.Wrt_data=state.MEM.ALUresult;

            // sd:rs2->(rs1+offset)
            if(state.MEM.wrt_mem)
            {
                myDataMem.writeDataMem(tmpALUResult,state.MEM.Store_data);
                state.WB.Wrt_data=state.MEM.Store_data;
            }
            state.WB.Rs=state.MEM.Rs;
            state.WB.Rt=state.MEM.Rt;
            state.WB.Wrt_reg_addr=state.MEM.Wrt_reg_addr;
            state.WB.wrt_enable=state.MEM.wrt_enable;
        }
        state.MEM.nop=state.EX.nop;


        /* --------------------- EX stage --------------------- */
        if(!state.EX.nop){
            bitset<64> data;
            data = state.EX.Read_data2;
            if(state.EX.is_I_type){
                data = state.EX.Imm;//直接将立即数读入
            }
            if(state.EX.wrt_mem)  //sd
            {
                data=state.EX.Imm;
            }

            //add
            if(state.EX.alu_op){
                state.MEM.ALUresult = bitset<64>(state.EX.Read_data1.to_ulong()+data.to_ulong());
//                if(cycle==11)
//                    cout<<state.MEM.ALUresult.to_ulong()<<endl;
            }
                //sub
            else{
                state.MEM.ALUresult = bitset<64>(state.EX.Read_data1.to_ulong() - data.to_ulong());
            }
        }
        state.MEM.Store_data = state.EX.Read_data2;
        state.MEM.Rt = state.EX.Rt;
        state.MEM.Rs = state.EX.Rs;
        state.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
        state.MEM.wrt_enable = state.EX.wrt_enable;
        state.MEM.rd_mem = state.EX.rd_mem;
        state.MEM.wrt_mem = state.EX.wrt_mem;

        //ld&sd相连
        if(state.MEM.Rt == state.WB.Wrt_reg_addr ){
            state.MEM.Store_data = state.WB.Wrt_data;
        }

        state.EX.nop = state.ID.nop;


        /* --------------------- ID stage --------------------- */
        //需要对指令进行译码
        if(!state.ID.nop){
            //判断是否是I-type
            // 不是
            if(state.ID.Instr.to_string().substr(25,7) != "0010011" && state.ID.Instr.to_string().substr(25,7) != "0000011"){
                state.EX.is_I_type = false;
                //确定rs1，rs2
                state.EX.Rs = bitset<5>(state.ID.Instr.to_string().substr(12,5));
                state.EX.Rt = bitset<5>(state.ID.Instr.to_string().substr(7,5));
                state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
                state.EX.Read_data2 = myRF.readRF(state.EX.Rt);
                state.EX.rd_mem= false;
                state.EX.wrt_enable = true;
            }
                // 是
            else{
                state.EX.is_I_type = true;
                // rs1
                state.EX.Rs = bitset<5>(state.ID.Instr.to_string().substr(12,5));
                state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
                // rd
                state.EX.Wrt_reg_addr = bitset<5>(state.ID.Instr.to_string().substr(20,5));
            }

            // ld
            if(state.ID.Instr.to_string().substr(25,7) == "0000011"){
                state.EX.rd_mem = true;
                state.EX.wrt_enable= true;
                state.EX.alu_op= true;
                state.EX.Imm = bitset<64>(state.ID.Instr.to_string().substr(0,12));//立即数
                if(state.EX.Imm[11]){//如果是负数
                    state.EX.Imm = bitset<64>(string(52,'1')+state.ID.Instr.to_string().substr(0,12));//立即数
                }
            }
            // sd
            if(state.ID.Instr.to_string().substr(25,7) == "0100011"){
                state.EX.Imm=bitset<64>(state.ID.Instr.to_string().substr(0, 7) +
                                        state.ID.Instr.to_string().substr(20, 5));
                state.EX.wrt_mem = true;
                state.EX.alu_op= true;
            }
            // R
            if(state.ID.Instr.to_string().substr(25,7) == "0110011"){
                state.EX.wrt_enable = true;
                state.EX.Wrt_reg_addr = bitset<5>(state.ID.Instr.to_string().substr(20,5));//rd
                // add
                if(state.ID.Instr.to_string().substr(0, 7) == string("0000000"))
                    state.EX.alu_op= true;
                // sub
                if(state.ID.Instr.to_string().substr(0, 7) == string("0100000"))
                    state.EX.alu_op= false;
            }
            // branch
            if(state.ID.Instr.to_string().substr(25,7) == "1100011")
            {
                state.EX.Imm=bitset<64>(state.ID.Instr.to_string().substr(0,1)+state.ID.Instr.to_string().substr(24,1)+state.ID.Instr.to_string().substr(1,6)+state.ID.Instr.to_string().substr(20,4));
            }


            //处理raw hazard，不包括load-use 冒险
            if(!state.EX.rd_mem){
                int flag=0;  // 是否处理过相邻
                if(state.MEM.wrt_enable){//需要写回数据,相邻的优先级应该大于次相邻
                    if(state.EX.Rs == state.MEM.Wrt_reg_addr){
                        flag=1;
                        state.EX.Read_data1 = state.MEM.ALUresult;
                        cout<<"RAW11 hazard cycle:"<<cycle<<" reg:"<<state.MEM.Wrt_reg_addr<<endl;
                    }
                    if(state.EX.Rt == state.MEM.Wrt_reg_addr){
                        flag=1;
                        state.EX.Read_data2 = state.MEM.ALUresult;
                        cout<<"RAW12 hazard cycle:"<<cycle<<" reg:"<<state.MEM.Wrt_reg_addr<<endl;
                    }

                }
                if(state.WB.wrt_enable&&flag==0){//需要写回数据
                    if(state.EX.Rs == state.WB.Wrt_reg_addr){
                        state.EX.Read_data1 = state.WB.Wrt_data;
                        cout<<"RAW21 hazard cycle:"<<cycle<<" reg:"<<state.MEM.Wrt_reg_addr<<endl;
                    }
                    if(state.EX.Rt == state.WB.Wrt_reg_addr){
                        state.EX.Read_data2 =state.WB.Wrt_data;
                        cout<<"RAW22 hazard cycle:"<<cycle<<" reg:"<<state.MEM.Wrt_reg_addr<<endl;
                    }

                }
            }
                //ld指令
            else
            {
                int flag=0;
                //ld作为consumer
                if(state.EX.Rs == state.MEM.Wrt_reg_addr){
                    // x0不可能被写，只能是初始化值还未修改
                    if(state.MEM.Wrt_reg_addr.to_string()!="00000"){
                        flag=1;
                        state.EX.Read_data1 = state.MEM.ALUresult;
                        cout<<"RAW31 hazard cycle:"<<cycle<<" reg:"<<state.MEM.Wrt_reg_addr<<endl;
                    }

                }
                if(state.EX.Rs == state.WB.Wrt_reg_addr&&flag==0){
                    if(state.WB.Wrt_reg_addr.to_string()!="00000"){
                        state.EX.Read_data1 = state.WB.Wrt_data;
                        cout<<"RAW32 hazard cycle:"<<cycle<<" reg:"<<state.MEM.Wrt_reg_addr<<endl;
                    }
                }
                // load-use
                if(state.EX.Wrt_reg_addr.to_string()==myInsMem.readInstr(state.IF.PC).to_string().substr(12,5)||
                   state.EX.Wrt_reg_addr.to_string()==myInsMem.readInstr(state.IF.PC).to_string().substr(7,5))
                {
                    if(state.EX.Wrt_reg_addr.to_string()!="00000")
                        // x0不可能被写，只能是初始化值还未修改
                    {
                        lu_flag=1;
                        cout<<"load-use hazard cycle:"<<cycle<<" reg:"<<state.EX.Wrt_reg_addr<<endl;
                        state.ID.nop = true;//flush
                    }
                }
            }

            // branch
            if(state.ID.Instr.to_string().substr(25,7) == "1100011"){
                cout<<"branch: "<<state.EX.Read_data1.to_ulong()<<' '<<state.EX.Read_data2.to_ulong()<<' '<<endl;
                if(state.EX.Read_data1 != state.EX.Read_data2){//不相等需要跳转
                    cout<<"imm: "<<state.EX.Imm<<' '<<endl;
                    string s = state.ID.Instr.to_string();
                    bitset<32> addressExtend;
                    addressExtend = bitset<32>(s.substr(0,1)+s.substr(24,1)+s.substr(1,6)+s.substr(20,4));
//                    cout<<"addressExtend: "<<addressExtend<<' '<<endl;
                    if(state.EX.Imm[11]){
                        addressExtend = bitset<32>(string(20,'1') + addressExtend.to_string().substr(20,12));//立即数
                        addressExtend.flip();
                        cout<<"addressExtend-after: "<<addressExtend<<' '<<endl;
                        state.IF.PC = bitset<32>(state.IF.PC.to_ulong()-(addressExtend.to_ulong()+1));//如果是负数
                    }
                    else{
                        state.IF.PC = bitset<32>(addressExtend.to_ulong()+state.IF.PC.to_ulong());
                    }
                    state.EX.nop = true;
                }
            }

        }
            // nop，清空所有控制信号
        else
        {
            state.EX.is_I_type= false;
            state.EX.rd_mem= false;
            state.EX.wrt_mem= false;
            state.EX.alu_op= false;
            state.EX.wrt_enable= false;
        }
        if(!lu_flag)
            state.ID.nop = state.IF.nop;



        /* --------------------- IF stage --------------------- */
        if(!state.IF.nop)
        {
            if(!lu_flag)
            {
                // 取指
                state.ID.Instr=myInsMem.readInstr(state.IF.PC);
                // 更新PC
                state.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
            }
            else
            {
                lu_flag=0;
            }
            //判断是否需要终止
            if(state.ID.Instr.to_string()=="11111111111111111111111111111111")
            {
                state.IF.nop= true;
                state.ID.nop= true;
            }
        }

        /* --------------------- Stall unit--------------------- */
        printState(state, cycle++);

        cout<<"cycle="<<cycle-1<<';'<<state.IF.nop<<' '<<state.ID.nop<<' '<<state.EX.nop<<' '<<state.MEM.nop<<' '<<state.WB.nop<<' '<<endl;
        if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop)
            break;

//        printState(newState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ...
//
//        cycle += 1;
//        state = newState; /*The end of the cycle and updates the current state with the values calculated in this cycle */

    }

    myRF.outputRF(); // dump RF;
    myDataMem.outputDataMem(); // dump data mem

    return 0;
}
