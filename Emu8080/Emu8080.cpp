// Emu8080.cpp : Defines the exported functions for the DLL application.
//

#include <iostream>
using std::cout;
using std::cin;
using std::cerr;

using std::hex;
using std::endl;

#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <iomanip>
using std::setw;

#include "EmuApi.h"
#include "Emu8080.hpp"
#include "Emu8080Datasheet.h"

#pragma region I/O Implementations

namespace Emu8080
{
	void SimpleOut(uint8_t data)
	{
		cout << data;
		//cerr << "OUT: " << hex << data << " " << (int) data << " " << data << endl;
	}

	uint8_t SimpleIn()
	{
		uint8_t data;
		cin >> data;
		return data;
	}
}

#pragma endregion

#pragma region API Definition

void* EmuInstantiate()
{
	Emu8080::Processor* newProcessor = new Emu8080::Processor();

	// Set up default I/O
	newProcessor->SetInputHandler(0xFF, Emu8080::SimpleIn);
	newProcessor->SetOutputHandler(0xFF, Emu8080::SimpleOut);

	return newProcessor;
}

void EmuDestroy(void* state)
{
	Emu8080::Processor* localState = (Emu8080::Processor*) state;

	delete localState;
}

void EmuExecute(void* state)
{
	Emu8080::Processor* localState = (Emu8080::Processor*) state;

	localState->Execute();
}

void EmuLoadMemory(void* state, void* src, EmuGenInt size, EmuGenInt offset)
{
	Emu8080::Processor* localState = (Emu8080::Processor*) state;

	if (size >= EMU8080_MEMORY_SIZE)
	{
		cerr << "Load Error!" << endl;
		cerr << "   Size " << size << " out of bounds." << endl;
		cerr << "   Maximum is " << EMU8080_MEMORY_SIZE - 1 << "." << endl;
	}
	else if (offset >= EMU8080_MEMORY_SIZE)
	{
		cerr << "Load Error!" << endl;
		cerr << "   Offset " << offset << " out of bounds." << endl;
		cerr << "   Maximum is " << EMU8080_MEMORY_SIZE - 1 << "." << endl;
	}
	else if ((size + offset) >= EMU8080_MEMORY_SIZE)
	{
		cerr << "Load Error!" << endl;
		cerr << "   Size (" << size << ") + Offset (" << offset << ") out of bounds." << endl;
		cerr << "   Maximum is " << EMU8080_MEMORY_SIZE - 1 << "." << endl;
	}
	else
	{
		localState->LoadMemory(src, (uint16_t) size, (uint16_t) offset);
	}
}

void EmuSetPc(void* state, EmuGenInt newPc)
{
	Emu8080::Processor* localState = (Emu8080::Processor*) state;

	if (newPc >= EMU8080_MEMORY_SIZE)
	{
		cerr << "PC value " << newPc << " out of bounds.  Maximum is " << EMU8080_MEMORY_SIZE - 1 << "." << endl;
	}
	else
	{
		localState->SetPc((uint16_t) newPc);
	}

}

EmuStatus EmuGetStatus(void* state)
{
	Emu8080::Processor* localState = (Emu8080::Processor*) state;

	return localState->GetStatus();
}

EmuGenInt EmuOpcodeCount()
{
	return 0x100;
}

EmuOpcodeData EmuGetOpcodeData(EmuGenInt opCode)
{
	switch (opCode)
	{
	case op8080_NOP: return{ "NOP", 1 };
	case op8080_LXI_B: return{ "LXI B", 3 };
	case op8080_STAX_B: return{ "STAX B", 1 };
	case op8080_INX_B: return{ "INX B", 1 };
	case op8080_INR_B: return{ "INR B", 1 };
	case op8080_DCR_B: return{ "DCR B", 1 };
	case op8080_MVI_B: return{ "MVI B", 2 };
	case op8080_RLC: return{ "RLC", 1 };
	case op8080_DAD_B: return{ "DAD B", 1 };
	case op8080_LDAX_B: return{ "LDAX B", 1 };
	case op8080_DCX_B: return{ "DCX B", 1 };
	case op8080_INR_C: return{ "INR C", 1 };
	case op8080_DCR_C: return{ "DCR C", 1 };
	case op8080_MVI_C: return{ "MVI C", 2 };
	case op8080_RRC: return{ "RRC", 1 };
	case op8080_LXI_D: return{ "LXI D", 3 };
	case op8080_STAX_D: return{ "STAX D", 1 };
	case op8080_INX_D: return{ "INX D", 1 };
	case op8080_INR_D: return{ "INR D", 1 };
	case op8080_DCR_D: return{ "DCR D", 1 };
	case op8080_MVI_D: return{ "MVI D", 2 };
	case op8080_RAL: return{ "RAL", 1 };
	case op8080_DAD_D: return{ "DAD D", 1 };
	case op8080_LDAX_D: return{ "LDAX D", 1 };
	case op8080_DCX_D: return{ "DCX D", 1 };
	case op8080_INR_E: return{ "INR E", 1 };
	case op8080_DCR_E: return{ "DCR E", 1 };
	case op8080_MVI_E: return{ "MVI E", 2 };
	case op8080_RAR: return{ "RAR", 1 };
	case op8080_RIM: return{ "RIM", 1 };
	case op8080_LXI_H: return{ "LXI H", 3 };
	case op8080_SHLD: return{ "SHLD", 3 };
	case op8080_INX_H: return{ "INX H", 1 };
	case op8080_INR_H: return{ "INR H", 1 };
	case op8080_DCR_H: return{ "DCR H", 1 };
	case op8080_MVI_H: return{ "MVI H", 2 };
	case op8080_DAA: return{ "DAA", 1 };
	case op8080_DAD_H: return{ "DAD H", 1 };
	case op8080_LHLD: return{ "LHLD", 3 };
	case op8080_DCX_H: return{ "DCX H", 1 };
	case op8080_INR_L: return{ "INR L", 1 };
	case op8080_DCR_L: return{ "DCR L", 1 };
	case op8080_MVI_L: return{ "MVI L", 2 };
	case op8080_CMA: return{ "CMA", 1 };
	case op8080_SIM: return{ "SIM", 1 };
	case op8080_LXI_SP: return{ "LXI SP", 3 };
	case op8080_STA: return{ "STA", 3 };
	case op8080_INX_SP: return{ "INX SP", 1 };
	case op8080_INR_M: return{ "INR M", 1 };
	case op8080_DCR_M: return{ "DCR M", 1 };
	case op8080_MVI_M: return{ "MVI M", 2 };
	case op8080_STC: return{ "STC", 1 };
	case op8080_DAD_SP: return{ "DAD SP", 1 };
	case op8080_LDA: return{ "LDA", 3 };
	case op8080_DCX_SP: return{ "DCX SP", 1 };
	case op8080_INR_A: return{ "INR A", 1 };
	case op8080_DCR_A: return{ "DCR A", 1 };
	case op8080_MVI_A: return{ "MVI A", 2 };
	case op8080_CMC: return{ "CMC", 1 };
	case op8080_MOV_B_B: return{ "MOV B B", 1 };
	case op8080_MOV_B_C: return{ "MOV B C", 1 };
	case op8080_MOV_B_D: return{ "MOV B D", 1 };
	case op8080_MOV_B_E: return{ "MOV B E", 1 };
	case op8080_MOV_B_H: return{ "MOV B H", 1 };
	case op8080_MOV_B_L: return{ "MOV B L", 1 };
	case op8080_MOV_B_M: return{ "MOV B M", 1 };
	case op8080_MOV_B_A: return{ "MOV B A", 1 };
	case op8080_MOV_C_B: return{ "MOV C B", 1 };
	case op8080_MOV_C_C: return{ "MOV C C", 1 };
	case op8080_MOV_C_D: return{ "MOV C D", 1 };
	case op8080_MOV_C_E: return{ "MOV C E", 1 };
	case op8080_MOV_C_H: return{ "MOV C H", 1 };
	case op8080_MOV_C_L: return{ "MOV C L", 1 };
	case op8080_MOV_C_M: return{ "MOV C M", 1 };
	case op8080_MOV_C_A: return{ "MOV C A", 1 };
	case op8080_MOV_D_B: return{ "MOV D B", 1 };
	case op8080_MOV_D_C: return{ "MOV D C", 1 };
	case op8080_MOV_D_D: return{ "MOV D D", 1 };
	case op8080_MOV_D_E: return{ "MOV D E", 1 };
	case op8080_MOV_D_H: return{ "MOV D H", 1 };
	case op8080_MOV_D_L: return{ "MOV D L", 1 };
	case op8080_MOV_D_M: return{ "MOV D M", 1 };
	case op8080_MOV_D_A: return{ "MOV D A", 1 };
	case op8080_MOV_E_B: return{ "MOV E B", 1 };
	case op8080_MOV_E_C: return{ "MOV E C", 1 };
	case op8080_MOV_E_D: return{ "MOV E D", 1 };
	case op8080_MOV_E_E: return{ "MOV E E", 1 };
	case op8080_MOV_E_H: return{ "MOV E H", 1 };
	case op8080_MOV_E_L: return{ "MOV E L", 1 };
	case op8080_MOV_E_M: return{ "MOV E M", 1 };
	case op8080_MOV_E_A: return{ "MOV E A", 1 };
	case op8080_MOV_H_B: return{ "MOV H B", 1 };
	case op8080_MOV_H_C: return{ "MOV H C", 1 };
	case op8080_MOV_H_D: return{ "MOV H D", 1 };
	case op8080_MOV_H_E: return{ "MOV H E", 1 };
	case op8080_MOV_H_H: return{ "MOV H H", 1 };
	case op8080_MOV_H_L: return{ "MOV H L", 1 };
	case op8080_MOV_H_M: return{ "MOV H M", 1 };
	case op8080_MOV_H_A: return{ "MOV H A", 1 };
	case op8080_MOV_L_B: return{ "MOV L B", 1 };
	case op8080_MOV_L_C: return{ "MOV L C", 1 };
	case op8080_MOV_L_D: return{ "MOV L D", 1 };
	case op8080_MOV_L_E: return{ "MOV L E", 1 };
	case op8080_MOV_L_H: return{ "MOV L H", 1 };
	case op8080_MOV_L_L: return{ "MOV L L", 1 };
	case op8080_MOV_L_M: return{ "MOV L M", 1 };
	case op8080_MOV_L_A: return{ "MOV L A", 1 };
	case op8080_MOV_M_B: return{ "MOV M B", 1 };
	case op8080_MOV_M_C: return{ "MOV M C", 1 };
	case op8080_MOV_M_D: return{ "MOV M D", 1 };
	case op8080_MOV_M_E: return{ "MOV M E", 1 };
	case op8080_MOV_M_H: return{ "MOV M H", 1 };
	case op8080_MOV_M_L: return{ "MOV M L", 1 };
	case op8080_HLT: return{ "HLT", 1 };
	case op8080_MOV_M_A: return{ "MOV M A", 1 };
	case op8080_MOV_A_B: return{ "MOV A B", 1 };
	case op8080_MOV_A_C: return{ "MOV A C", 1 };
	case op8080_MOV_A_D: return{ "MOV A D", 1 };
	case op8080_MOV_A_E: return{ "MOV A E", 1 };
	case op8080_MOV_A_H: return{ "MOV A H", 1 };
	case op8080_MOV_A_L: return{ "MOV A L", 1 };
	case op8080_MOV_A_M: return{ "MOV A M", 1 };
	case op8080_MOV_A_A: return{ "MOV A A", 1 };
	case op8080_ADD_B: return{ "ADD B", 1 };
	case op8080_ADD_C: return{ "ADD C", 1 };
	case op8080_ADD_D: return{ "ADD D", 1 };
	case op8080_ADD_E: return{ "ADD E", 1 };
	case op8080_ADD_H: return{ "ADD H", 1 };
	case op8080_ADD_L: return{ "ADD L", 1 };
	case op8080_ADD_M: return{ "ADD M", 1 };
	case op8080_ADD_A: return{ "ADD A", 1 };
	case op8080_ADC_B: return{ "ADC B", 1 };
	case op8080_ADC_C: return{ "ADC C", 1 };
	case op8080_ADC_D: return{ "ADC D", 1 };
	case op8080_ADC_E: return{ "ADC E", 1 };
	case op8080_ADC_H: return{ "ADC H", 1 };
	case op8080_ADC_L: return{ "ADC L", 1 };
	case op8080_ADC_M: return{ "ADC M", 1 };
	case op8080_ADC_A: return{ "ADC A", 1 };
	case op8080_SUB_B: return{ "SUB B", 1 };
	case op8080_SUB_C: return{ "SUB C", 1 };
	case op8080_SUB_D: return{ "SUB D", 1 };
	case op8080_SUB_E: return{ "SUB E", 1 };
	case op8080_SUB_H: return{ "SUB H", 1 };
	case op8080_SUB_L: return{ "SUB L", 1 };
	case op8080_SUB_M: return{ "SUB M", 1 };
	case op8080_SUB_A: return{ "SUB A", 1 };
	case op8080_SBB_B: return{ "SBB B", 1 };
	case op8080_SBB_C: return{ "SBB C", 1 };
	case op8080_SBB_D: return{ "SBB D", 1 };
	case op8080_SBB_E: return{ "SBB E", 1 };
	case op8080_SBB_H: return{ "SBB H", 1 };
	case op8080_SBB_L: return{ "SBB L", 1 };
	case op8080_SBB_M: return{ "SBB M", 1 };
	case op8080_SBB_A: return{ "SBB A", 1 };
	case op8080_ANA_B: return{ "ANA B", 1 };
	case op8080_ANA_C: return{ "ANA C", 1 };
	case op8080_ANA_D: return{ "ANA D", 1 };
	case op8080_ANA_E: return{ "ANA E", 1 };
	case op8080_ANA_H: return{ "ANA H", 1 };
	case op8080_ANA_L: return{ "ANA L", 1 };
	case op8080_ANA_M: return{ "ANA M", 1 };
	case op8080_ANA_A: return{ "ANA A", 1 };
	case op8080_XRA_B: return{ "XRA B", 1 };
	case op8080_XRA_C: return{ "XRA C", 1 };
	case op8080_XRA_D: return{ "XRA D", 1 };
	case op8080_XRA_E: return{ "XRA E", 1 };
	case op8080_XRA_H: return{ "XRA H", 1 };
	case op8080_XRA_L: return{ "XRA L", 1 };
	case op8080_XRA_M: return{ "XRA M", 1 };
	case op8080_XRA_A: return{ "XRA A", 1 };
	case op8080_ORA_B: return{ "ORA B", 1 };
	case op8080_ORA_C: return{ "ORA C", 1 };
	case op8080_ORA_D: return{ "ORA D", 1 };
	case op8080_ORA_E: return{ "ORA E", 1 };
	case op8080_ORA_H: return{ "ORA H", 1 };
	case op8080_ORA_L: return{ "ORA L", 1 };
	case op8080_ORA_M: return{ "ORA M", 1 };
	case op8080_ORA_A: return{ "ORA A", 1 };
	case op8080_CMP_B: return{ "CMP B", 1 };
	case op8080_CMP_C: return{ "CMP C", 1 };
	case op8080_CMP_D: return{ "CMP D", 1 };
	case op8080_CMP_E: return{ "CMP E", 1 };
	case op8080_CMP_H: return{ "CMP H", 1 };
	case op8080_CMP_L: return{ "CMP L", 1 };
	case op8080_CMP_M: return{ "CMP M", 1 };
	case op8080_CMP_A: return{ "CMP A", 1 };
	case op8080_RNZ: return{ "RNZ", 1 };
	case op8080_POP_B: return{ "POP B", 1 };
	case op8080_JNZ: return{ "JNZ", 3 };
	case op8080_JMP: return{ "JMP", 3 };
	case op8080_CNZ: return{ "CNZ", 3 };
	case op8080_PUSH_B: return{ "PUSH B", 1 };
	case op8080_ADI: return{ "ADI", 2 };
	case op8080_RST_0: return{ "RST 0", 1 };
	case op8080_RZ: return{ "RZ", 1 };
	case op8080_RET: return{ "RET", 1 };
	case op8080_JZ: return{ "JZ", 3 };
	case op8080_CZ: return{ "CZ", 3 };
	case op8080_CALL: return{ "CALL", 3 };
	case op8080_ACI: return{ "ACI", 2 };
	case op8080_RST_1: return{ "RST 1", 1 };
	case op8080_RNC: return{ "RNC", 1 };
	case op8080_POP_D: return{ "POP D", 1 };
	case op8080_JNC: return{ "JNC", 3 };
	case op8080_OUT: return{ "OUT", 2 };
	case op8080_CNC: return{ "CNC", 3 };
	case op8080_PUSH_D: return{ "PUSH D", 1 };
	case op8080_SUI: return{ "SUI", 2 };
	case op8080_RST_2: return{ "RST 2", 1 };
	case op8080_RC: return{ "RC", 1 };
	case op8080_JC: return{ "JC", 3 };
	case op8080_IN: return{ "IN", 2 };
	case op8080_CC: return{ "CC", 3 };
	case op8080_SBI: return{ "SBI", 2 };
	case op8080_RST_3: return{ "RST 3", 1 };
	case op8080_RPO: return{ "RPO", 1 };
	case op8080_POP_H: return{ "POP H", 1 };
	case op8080_JPO: return{ "JPO", 3 };
	case op8080_XTHL: return{ "XTHL", 1 };
	case op8080_CPO: return{ "CPO", 3 };
	case op8080_PUSH_H: return{ "PUSH H", 1 };
	case op8080_ANI: return{ "ANI", 2 };
	case op8080_RST_4: return{ "RST 4", 1 };
	case op8080_RPE: return{ "RPE", 1 };
	case op8080_PCHL: return{ "PCHL", 1 };
	case op8080_JPE: return{ "JPE", 3 };
	case op8080_XCHG: return{ "XCHG", 1 };
	case op8080_CPE: return{ "CPE", 3 };
	case op8080_XRI: return{ "XRI", 2 };
	case op8080_RST_5: return{ "RST 5", 1 };
	case op8080_RP: return{ "RP", 1 };
	case op8080_POP_PSW: return{ "POP PSW", 1 };
	case op8080_JP: return{ "JP", 3 };
	case op8080_DI: return{ "DI", 1 };
	case op8080_CP: return{ "CP", 3 };
	case op8080_PUSH_PSW: return{ "PUSH PSW", 1 };
	case op8080_ORI: return{ "ORI", 2 };
	case op8080_RST_6: return{ "RST 6", 1 };
	case op8080_RM: return{ "RM", 1 };
	case op8080_SPHL: return{ "SPHL", 1 };
	case op8080_JM: return{ "JM", 3 };
	case op8080_EI: return{ "EI", 1 };
	case op8080_CM: return{ "CM", 3 };
	case op8080_CPI: return{ "CPI", 2 };
	case op8080_RST_7: return{ "RST 7", 1 };
	default: return{ "NOT FOUND", 0 };
	}
}

void EmuDecompileNext(void* state, char output[EMU_MAX_STRINGBUF_SIZE])
{
	Emu8080::Processor* localState = (Emu8080::Processor*) state;

	localState->DecompileNext(output);
}

#pragma endregion

#pragma region Class Definition

namespace Emu8080
{
	Processor::Processor()
	{
		this->a = 0;
		this->b = 0;
		this->c = 0;
		this->d = 0;
		this->e = 0;
		this->l = 0;
		this->sp = 0;
		this->pc = 0;
		this->GetCC() = 0;

		this->pc_incr = 0;
		this->int_enable = true;

		memset(this->memory, 0, EMU8080_MEMORY_SIZE);
		memset(this->input_handlers, NULL, 0x100);
		memset(this->output_handlers, NULL, 0x100);

		this->status = statusRun;
	}

	void Processor::LoadMemory(void* src, uint16_t size, uint16_t offset)
	{
		memcpy(this->memory + offset, src, size);
	}

	void Processor::SetPc(uint16_t newPc)
	{
		this->pc = newPc;
	}

	EmuStatus Processor::GetStatus()
	{
		return this->status;
	}

	void Processor::SetInputHandler(uint8_t id, InputHandler callback)
	{
		this->input_handlers[id] = callback;
	}

	void Processor::SetOutputHandler(uint8_t id, OutputHandler callback)
	{
		this->output_handlers[id] = callback;
	}

#pragma region Execute Helpers

	int Parity(uint64_t x)
	{
		x ^= x >> 32;
		x ^= x >> 16;
		x ^= x >> 8;
		x ^= x >> 4;
		x ^= x >> 2;
		x ^= x >> 1;
		return (~x) & 1;
	}

	uint8_t High8(uint16_t comp)
	{
		return (comp & 0xFF00) >> 8;
	}

	uint8_t Low8(uint16_t comp)
	{
		return comp & 0x00FF;
	}

	uint16_t Compose16(uint8_t h, uint8_t l)
	{
		return (h << 8) | l;
	}

	void Compose16(uint16_t& comp, uint8_t h, uint8_t l)
	{
		comp = Compose16(h, l);
	}

	void Decompose16(uint16_t comp, uint8_t& h, uint8_t& l)
	{
		h = High8(comp);
		l = Low8(comp);
	}

	uint8_t& Processor::Offset8(uint16_t offset)
	{
		return *(this->memory + offset);
	}

	uint8_t& Processor::Offset8(uint8_t h, uint8_t l)
	{
		return this->Offset8(Compose16(h, l));
	}

	uint8_t& Processor::Offset8()
	{
		return this->Offset8(this->h, this->l);
	}

	uint8_t Processor::UpdateCnd(uint16_t value)
	{
		this->cc.z = ((value & 0xff) == 0);
		this->cc.s = ((value & 0x80) != 0);
		this->cc.p = Parity(value & 0xff);
		this->cc.cy = (value > 0xff);
		return value & 0xff;
	}

	void Processor::Add16(uint16_t other)
	{
		uint32_t answer = (uint32_t) Compose16(this->h, this->l) + (uint32_t) other;
		this->cc.cy = (answer > 0xffff);
		Decompose16(answer & 0xffff, this->h, this->l);
	}

	void Processor::Add8(uint8_t& target, uint16_t other)
	{
		this->cc.ac = ((target & 0xF) + (other & 0xF) > 0xf);
		target = this->UpdateCnd((uint16_t) target + other);
	}

	void Processor::Add8(uint16_t other)
	{
		this->Add8(this->a, other);
	}

	void Processor::Cmp8(uint16_t other)
	{
		this->cc.ac = ((this->a & 0xF) - (other & 0xF) > 0xf);
		this->UpdateCnd((uint16_t) this->a - other);
	}

	void Processor::RotLeft(bool useCarry)
	{
		uint8_t x = this->a;

		if (useCarry)
			this->a = (this->cc.cy) | (x << 1);
		else
			this->a = ((x & 0x80) >> 7) | (x << 1);

		this->cc.cy = (0 != (x & 0x80));
	}

	void Processor::RotRight(bool useCarry)
	{
		uint8_t x = this->a;

		if (useCarry)
			this->a = (this->cc.cy << 7) | (x >> 1);
		else
			this->a = ((x & 0x01) << 7) | (x >> 1);

		this->cc.cy = (0 != (x & 0x01));
	}

	void Processor::Push8(uint8_t data)
	{
		this->memory[--this->sp] = data;
	}

	void Processor::Push16(uint16_t data)
	{
		this->Push8(High8(data));
		this->Push8(Low8(data));
	}

	uint8_t Processor::Pop8()
	{
		return this->memory[this->sp++];
	}

	uint16_t Processor::Pop16()
	{
		uint8_t l = this->Pop8();
		uint8_t h = this->Pop8();
		return Compose16(h, l);
	}

	void Processor::Jmp(uint16_t dst)
	{
		this->pc = dst;
		this->pc_incr = 0;
	}

	void Processor::Call(uint16_t dst)
	{
		this->Push16(this->pc + 3);
		this->Jmp(dst);
	}

	void Processor::Ret()
	{
		this->pc = this->Pop16();
		this->pc_incr = 0;
	}

	uint8_t* Processor::OpcodePtr()
	{
		return this->memory + this->pc;
	}

	uint8_t* Processor::StackPtr()
	{
		return this->memory + this->sp;
	}

	uint8_t Processor::Imm8()
	{
		return *(this->OpcodePtr() + 1);
	}

	uint16_t Processor::Imm16()
	{
		return Compose16(*(this->OpcodePtr() + 2), *(this->OpcodePtr() + 1));
	}

	uint8_t& Processor::GetCC()
	{
		return *((uint8_t*) (&this->cc));
	}

	bool Processor::IsZ()
	{
		return this->cc.z == 1;
	}

	bool Processor::IsNZ()
	{
		return this->cc.z == 0;
	}

	bool Processor::IsC()
	{
		return this->cc.cy == 1;
	}

	bool Processor::IsNC()
	{
		return this->cc.cy == 0;
	}

	bool Processor::IsPO()
	{
		return this->cc.p == 0;
	}

	bool Processor::IsPE()
	{
		return this->cc.p == 1;
	}

	bool Processor::IsP()
	{
		return this->cc.s == 0;
	}

	bool Processor::IsM()
	{
		return this->cc.s == 1;
	}

	void Swap8(uint8_t& left, uint8_t& right)
	{
		int temp = left;
		left = right;
		right = temp;
	}

#pragma endregion

	void Processor::Execute()
	{
		if (this->status == statusRun)
		{
			uint8_t *opcode = this->OpcodePtr();
			this->pc_incr = (uint16_t)EmuGetOpcodeData(*opcode).size;

			switch (*opcode)
			{

#pragma region Moves, Stores and Loads

			case op8080_MOV_B_B: this->b = this->b; break;
			case op8080_MOV_B_C: this->b = this->c; break;
			case op8080_MOV_B_D: this->b = this->d; break;
			case op8080_MOV_B_E: this->b = this->e; break;
			case op8080_MOV_B_H: this->b = this->h; break;
			case op8080_MOV_B_L: this->b = this->l; break;
			case op8080_MOV_B_M: this->b = this->Offset8(); break;
			case op8080_MOV_B_A: this->b = this->a; break;

			case op8080_MOV_C_B: this->c = this->b; break;
			case op8080_MOV_C_C: this->c = this->c; break;
			case op8080_MOV_C_D: this->c = this->d; break;
			case op8080_MOV_C_E: this->c = this->e; break;
			case op8080_MOV_C_H: this->c = this->h; break;
			case op8080_MOV_C_L: this->c = this->l; break;
			case op8080_MOV_C_M: this->c = this->Offset8(); break;
			case op8080_MOV_C_A: this->c = this->a; break;

			case op8080_MOV_D_B: this->d = this->b; break;
			case op8080_MOV_D_C: this->d = this->c; break;
			case op8080_MOV_D_D: this->d = this->d; break;
			case op8080_MOV_D_E: this->d = this->e; break;
			case op8080_MOV_D_H: this->d = this->h; break;
			case op8080_MOV_D_L: this->d = this->l; break;
			case op8080_MOV_D_M: this->d = this->Offset8(); break;
			case op8080_MOV_D_A: this->d = this->a; break;

			case op8080_MOV_E_B: this->e = this->b; break;
			case op8080_MOV_E_C: this->e = this->c; break;
			case op8080_MOV_E_D: this->e = this->d; break;
			case op8080_MOV_E_E: this->e = this->e; break;
			case op8080_MOV_E_H: this->e = this->h; break;
			case op8080_MOV_E_L: this->e = this->l; break;
			case op8080_MOV_E_M: this->e = this->Offset8(); break;
			case op8080_MOV_E_A: this->e = this->a; break;

			case op8080_MOV_H_B: this->h = this->b; break;
			case op8080_MOV_H_C: this->h = this->c; break;
			case op8080_MOV_H_D: this->h = this->d; break;
			case op8080_MOV_H_E: this->h = this->e; break;
			case op8080_MOV_H_H: this->h = this->h; break;
			case op8080_MOV_H_L: this->h = this->l; break;
			case op8080_MOV_H_M: this->h = this->Offset8(); break;
			case op8080_MOV_H_A: this->h = this->a; break;

			case op8080_MOV_L_B: this->l = this->b; break;
			case op8080_MOV_L_C: this->l = this->c; break;
			case op8080_MOV_L_D: this->l = this->d; break;
			case op8080_MOV_L_E: this->l = this->e; break;
			case op8080_MOV_L_H: this->l = this->h; break;
			case op8080_MOV_L_L: this->l = this->l; break;
			case op8080_MOV_L_M: this->l = this->Offset8(); break;
			case op8080_MOV_L_A: this->l = this->a; break;

			case op8080_MOV_M_B: this->Offset8() = this->b; break;
			case op8080_MOV_M_C: this->Offset8() = this->c; break;
			case op8080_MOV_M_D: this->Offset8() = this->d; break;
			case op8080_MOV_M_E: this->Offset8() = this->e; break;
			case op8080_MOV_M_H: this->Offset8() = this->h; break;
			case op8080_MOV_M_L: this->Offset8() = this->l; break;
			case op8080_MOV_M_A: this->Offset8() = this->a; break;

			case op8080_MOV_A_B: this->a = this->b; break;
			case op8080_MOV_A_C: this->a = this->c; break;
			case op8080_MOV_A_D: this->a = this->d; break;
			case op8080_MOV_A_E: this->a = this->e; break;
			case op8080_MOV_A_H: this->a = this->h; break;
			case op8080_MOV_A_L: this->a = this->l; break;
			case op8080_MOV_A_M: this->a = this->Offset8(); break;
			case op8080_MOV_A_A: this->a = this->a; break;

			case op8080_MVI_B: this->b = this->Imm8(); break;
			case op8080_MVI_C: this->c = this->Imm8(); break;
			case op8080_MVI_D: this->d = this->Imm8(); break;
			case op8080_MVI_E: this->e = this->Imm8(); break;
			case op8080_MVI_H: this->h = this->Imm8(); break;
			case op8080_MVI_L: this->l = this->Imm8(); break;
			case op8080_MVI_M: this->Offset8() = this->Imm8(); break;
			case op8080_MVI_A: this->a = this->Imm8(); break;

			case op8080_STA: this->memory[this->Imm16()] = this->a; break;

			case op8080_LDA: this->a = this->memory[this->Imm16()]; break;

			case op8080_STAX_B: this->Offset8(this->b, this->c) = this->a; break;
			case op8080_STAX_D: this->Offset8(this->d, this->e) = this->a; break;

			case op8080_LDAX_B: this->a = this->Offset8(this->b, this->c); break;
			case op8080_LDAX_D: this->a = this->Offset8(this->d, this->e); break;

			case op8080_SHLD:
				this->memory[this->Imm16() + 1] = this->h;
				this->memory[this->Imm16()] = this->l;
				break;

			case op8080_LHLD:
				this->h = this->memory[this->Imm16() + 1];
				this->l = this->memory[this->Imm16()];
				break;

			case op8080_LXI_B: Decompose16(Imm16(), this->b, this->c); break;
			case op8080_LXI_D: Decompose16(Imm16(), this->d, this->e); break;
			case op8080_LXI_H: Decompose16(Imm16(), this->h, this->l); break;
			case op8080_LXI_SP: this->sp = this->Imm16(); break;

#pragma endregion

#pragma region 8-bit Arithmetic

			case op8080_INR_B: this->Add8(this->b, 1); break;
			case op8080_INR_C: this->Add8(this->c, 1); break;
			case op8080_INR_D: this->Add8(this->d, 1); break;
			case op8080_INR_E: this->Add8(this->e, 1); break;
			case op8080_INR_H: this->Add8(this->h, 1); break;
			case op8080_INR_L: this->Add8(this->l, 1); break;
			case op8080_INR_M: this->Add8(this->Offset8(), 1); break;
			case op8080_INR_A: this->Add8(this->a, 1); break;

			case op8080_ADD_B: this->Add8(this->b); break;
			case op8080_ADD_C: this->Add8(this->c); break;
			case op8080_ADD_D: this->Add8(this->d); break;
			case op8080_ADD_E: this->Add8(this->e); break;
			case op8080_ADD_H: this->Add8(this->h); break;
			case op8080_ADD_L: this->Add8(this->l); break;
			case op8080_ADD_M: this->Add8(this->Offset8()); break;
			case op8080_ADD_A: this->Add8(this->a); break;

			case op8080_ADI: this->Add8(this->Imm8()); break;

			case op8080_ADC_B: this->Add8(this->cc.cy + this->b); break;
			case op8080_ADC_C: this->Add8(this->cc.cy + this->c); break;
			case op8080_ADC_D: this->Add8(this->cc.cy + this->d); break;
			case op8080_ADC_E: this->Add8(this->cc.cy + this->e); break;
			case op8080_ADC_H: this->Add8(this->cc.cy + this->h); break;
			case op8080_ADC_L: this->Add8(this->cc.cy + this->l); break;
			case op8080_ADC_M: this->Add8(this->cc.cy + this->Offset8()); break;
			case op8080_ADC_A: this->Add8(this->cc.cy + this->a); break;

			case op8080_ACI: this->Add8(this->cc.cy + this->Imm8()); break;

			case op8080_DCR_B: this->Add8(this->b, -1); break;
			case op8080_DCR_C: this->Add8(this->c, -1); break;
			case op8080_DCR_D: this->Add8(this->d, -1); break;
			case op8080_DCR_E: this->Add8(this->e, -1); break;
			case op8080_DCR_H: this->Add8(this->h, -1); break;
			case op8080_DCR_L: this->Add8(this->l, -1); break;
			case op8080_DCR_M: this->Add8(this->Offset8(), -1); break;
			case op8080_DCR_A: this->Add8(this->a, -1); break;

			case op8080_SUB_B: this->Add8(-this->b); break;
			case op8080_SUB_C: this->Add8(-this->c); break;
			case op8080_SUB_D: this->Add8(-this->d); break;
			case op8080_SUB_E: this->Add8(-this->e); break;
			case op8080_SUB_H: this->Add8(-this->h); break;
			case op8080_SUB_L: this->Add8(-this->l); break;
			case op8080_SUB_M: this->Add8(-this->Offset8()); break;
			case op8080_SUB_A: this->Add8(-this->a); break;

			case op8080_SUI: this->Add8(-this->Imm8()); break;

			case op8080_SBB_B: this->Add8(-this->cc.cy - this->b); break;
			case op8080_SBB_C: this->Add8(-this->cc.cy - this->c); break;
			case op8080_SBB_D: this->Add8(-this->cc.cy - this->d); break;
			case op8080_SBB_E: this->Add8(-this->cc.cy - this->e); break;
			case op8080_SBB_H: this->Add8(-this->cc.cy - this->h); break;
			case op8080_SBB_L: this->Add8(-this->cc.cy - this->l); break;
			case op8080_SBB_M: this->Add8(-this->cc.cy - this->Offset8()); break;
			case op8080_SBB_A: this->Add8(-this->cc.cy - this->a); break;

			case op8080_SBI: this->Add8(-this->cc.cy - this->Imm8()); break;

			case op8080_CMP_B: this->Cmp8(this->b); break;
			case op8080_CMP_C: this->Cmp8(this->c); break;
			case op8080_CMP_D: this->Cmp8(this->d); break;
			case op8080_CMP_E: this->Cmp8(this->e); break;
			case op8080_CMP_H: this->Cmp8(this->h); break;
			case op8080_CMP_L: this->Cmp8(this->l); break;
			case op8080_CMP_M: this->Cmp8(this->Offset8()); break;
			case op8080_CMP_A: this->Cmp8(this->a); break;

			case op8080_CPI: this->Cmp8(this->Imm8()); break;

			case op8080_DAA:
			{
				uint16_t val = this->a;

				if (((val & 0x0F) > 0x09) || (this->cc.ac == 1))
				{
					val += 0x06;
				}

				if (((val & 0xF0) > 0x90) || (this->cc.cy == 1))
				{
					val += 0x60;
					this->cc.cy = 1;
				}
				else
				{
					this->cc.cy = 0;
				}

				this->a = val & 0xFF;
			} break;

			case op8080_RLC: this->RotLeft(false); break;
			case op8080_RAL: this->RotLeft(true); break;
			case op8080_RRC: this->RotRight(false); break;
			case op8080_RAR: this->RotRight(true); break;

#pragma endregion

#pragma region 16-bit Arithmetic

			// INX
			case op8080_INX_B: Decompose16(Compose16(this->b, this->c) + 1, this->b, this->c); break;
			case op8080_INX_D: Decompose16(Compose16(this->d, this->e) + 1, this->d, this->e); break;
			case op8080_INX_H: Decompose16(Compose16(this->h, this->l) + 1, this->h, this->l); break;
			case op8080_INX_SP: this->sp += 1; break;

			case op8080_DCX_B: Decompose16(Compose16(this->b, this->c) - 1, this->b, this->c); break;
			case op8080_DCX_D: Decompose16(Compose16(this->d, this->e) - 1, this->d, this->e); break;
			case op8080_DCX_H: Decompose16(Compose16(this->h, this->l) - 1, this->h, this->l); break;
			case op8080_DCX_SP: this->sp -= 1; break;

			case op8080_DAD_B: this->Add16(Compose16(this->b, this->c)); break;
			case op8080_DAD_D: this->Add16(Compose16(this->d, this->e)); break;
			case op8080_DAD_H: this->Add16(Compose16(this->h, this->l)); break;
			case op8080_DAD_SP: this->Add16(this->sp); break;

			case op8080_XCHG: Swap8(this->h, this->d); Swap8(this->l, this->e); break;
			case op8080_XTHL: Swap8(this->h, *(this->StackPtr() + 1)); Swap8(this->l, *this->StackPtr()); break;

#pragma endregion

#pragma region Bitfield Arithmetic

			case op8080_ANA_B: this->a = this->UpdateCnd((uint16_t) this->a & this->b); break;
			case op8080_ANA_C: this->a = this->UpdateCnd((uint16_t) this->a & this->c); break;
			case op8080_ANA_D: this->a = this->UpdateCnd((uint16_t) this->a & this->d); break;
			case op8080_ANA_E: this->a = this->UpdateCnd((uint16_t) this->a & this->e); break;
			case op8080_ANA_H: this->a = this->UpdateCnd((uint16_t) this->a & this->h); break;
			case op8080_ANA_L: this->a = this->UpdateCnd((uint16_t) this->a & this->l); break;
			case op8080_ANA_M: this->a = this->UpdateCnd((uint16_t) this->a & this->Offset8()); break;
			case op8080_ANA_A: this->a = this->UpdateCnd((uint16_t) this->a & this->a); break;

			case op8080_ANI: this->a = this->UpdateCnd((uint16_t) this->a & this->Imm8()); break;

			case op8080_XRA_B: this->a = this->UpdateCnd((uint16_t) this->a ^ this->b); break;
			case op8080_XRA_C: this->a = this->UpdateCnd((uint16_t) this->a ^ this->c); break;
			case op8080_XRA_D: this->a = this->UpdateCnd((uint16_t) this->a ^ this->d); break;
			case op8080_XRA_E: this->a = this->UpdateCnd((uint16_t) this->a ^ this->e); break;
			case op8080_XRA_H: this->a = this->UpdateCnd((uint16_t) this->a ^ this->h); break;
			case op8080_XRA_L: this->a = this->UpdateCnd((uint16_t) this->a ^ this->l); break;
			case op8080_XRA_M: this->a = this->UpdateCnd((uint16_t) this->a ^ this->Offset8()); break;
			case op8080_XRA_A: this->a = this->UpdateCnd((uint16_t) this->a ^ this->a); break;

			case op8080_XRI: this->a = this->UpdateCnd((uint16_t) this->a ^ this->Imm8()); break;

			case op8080_ORA_B: this->a = this->UpdateCnd((uint16_t) this->a | this->b); break;
			case op8080_ORA_C: this->a = this->UpdateCnd((uint16_t) this->a | this->c); break;
			case op8080_ORA_D: this->a = this->UpdateCnd((uint16_t) this->a | this->d); break;
			case op8080_ORA_E: this->a = this->UpdateCnd((uint16_t) this->a | this->e); break;
			case op8080_ORA_H: this->a = this->UpdateCnd((uint16_t) this->a | this->h); break;
			case op8080_ORA_L: this->a = this->UpdateCnd((uint16_t) this->a | this->l); break;
			case op8080_ORA_M: this->a = this->UpdateCnd((uint16_t) this->a | this->Offset8()); break;
			case op8080_ORA_A: this->a = this->UpdateCnd((uint16_t) this->a | this->a); break;

			case op8080_ORI: this->a = this->UpdateCnd((uint16_t) this->a | this->Imm8()); break;

			case op8080_CMA: this->a = (~this->a); break;

#pragma endregion

#pragma region Stack

			case op8080_PUSH_B: this->Push16(Compose16(this->b, this->c)); break;
			case op8080_PUSH_D: this->Push16(Compose16(this->d, this->e)); break;
			case op8080_PUSH_H: this->Push16(Compose16(this->h, this->l)); break;
			case op8080_PUSH_PSW: this->Push16(Compose16(this->a, this->GetCC())); break;

			case op8080_POP_B: Decompose16(this->Pop16(), this->b, this->c); break;
			case op8080_POP_D: Decompose16(this->Pop16(), this->d, this->e); break;
			case op8080_POP_H: Decompose16(this->Pop16(), this->h, this->l); break;
			case op8080_POP_PSW: Decompose16(this->Pop16(), this->a, this->GetCC()); break;

#pragma endregion

#pragma region Branching

			case op8080_JMP: this->Jmp(this->Imm16()); break;
			case op8080_JNZ: if (this->IsNZ()) this->Jmp(this->Imm16()); break;
			case op8080_JZ:  if (this->IsZ()) this->Jmp(this->Imm16()); break;
			case op8080_JNC: if (this->IsNC()) this->Jmp(this->Imm16()); break;
			case op8080_JC:  if (this->IsC()) this->Jmp(this->Imm16()); break;
			case op8080_JPO: if (this->IsPO()) this->Jmp(this->Imm16()); break;
			case op8080_JPE: if (this->IsPE()) this->Jmp(this->Imm16()); break;
			case op8080_JP:  if (this->IsP()) this->Jmp(this->Imm16()); break;
			case op8080_JM:  if (this->IsM()) this->Jmp(this->Imm16()); break;

			case op8080_CALL: this->Call(this->Imm16()); break;
			case op8080_CNZ:  if (this->IsNZ()) this->Call(this->Imm16()); break;
			case op8080_CZ:   if (this->IsZ()) this->Call(this->Imm16()); break;
			case op8080_CNC:  if (this->IsNC()) this->Call(this->Imm16()); break;
			case op8080_CC:   if (this->IsC()) this->Call(this->Imm16()); break;
			case op8080_CPO:  if (this->IsPO()) this->Call(this->Imm16()); break;
			case op8080_CPE:  if (this->IsPE()) this->Call(this->Imm16()); break;
			case op8080_CP:   if (this->IsP()) this->Call(this->Imm16()); break;
			case op8080_CM:   if (this->IsM()) this->Call(this->Imm16()); break;

			case op8080_RET: this->Ret(); break;
			case op8080_RNZ: if (this->IsNZ()) this->Ret(); break;
			case op8080_RZ:  if (this->IsZ()) this->Ret(); break;
			case op8080_RNC: if (this->IsNC()) this->Ret(); break;
			case op8080_RC:  if (this->IsC()) this->Ret(); break;
			case op8080_RPO: if (this->IsPO()) this->Ret(); break;
			case op8080_RPE: if (this->IsPE()) this->Ret(); break;
			case op8080_RP:  if (this->IsP()) this->Ret(); break;
			case op8080_RM:  if (this->IsM()) this->Ret(); break;

			case op8080_RST_0: this->Call(0x00); break;
			case op8080_RST_1: this->Call(0x08); break;
			case op8080_RST_2: this->Call(0x10); break;
			case op8080_RST_3: this->Call(0x18); break;
			case op8080_RST_4: this->Call(0x20); break;
			case op8080_RST_5: this->Call(0x28); break;
			case op8080_RST_6: this->Call(0x30); break;
			case op8080_RST_7: this->Call(0x38); break;

#pragma endregion

#pragma region I/O

			case op8080_OUT:
				if (this->output_handlers[this->Imm8()] != NULL)
					this->output_handlers[this->Imm8()](this->a);
				break;

			case op8080_IN:
				if (this->input_handlers[this->Imm8()] != NULL)
					this->a = this->input_handlers[this->Imm8()]();
				break;

#pragma endregion

#pragma region Misc

			case op8080_STC: this->cc.cy = 1; break;
			case op8080_CMC: this->cc.cy = 1 - this->cc.cy; break;

			case op8080_DI: this->int_enable = false; break;
			case op8080_EI: this->int_enable = true; break;

			case op8080_SPHL: Compose16(this->sp, this->h, this->l); break;
			case op8080_PCHL: this->Jmp(Compose16(this->h, this->l)); break;

#pragma endregion

			case op8080_NOP: break;

			case op8080_HLT:
				this->pc_incr = 0;
				this->status = statusHalt;
				break;

			default:
				this->pc_incr = 0;
				this->status = statusUnimplementedInstruction;
				break;
			}

			this->pc += this->pc_incr;
		}
	}

	void Processor::DecompileNext(char output[EMU_MAX_STRINGBUF_SIZE])
	{
		stringstream ss;

		EmuOpcodeData data = EmuGetOpcodeData(*this->OpcodePtr());

		ss << "[" << hex << setw(4) << this->pc << "]:" << data.name;
		
		if (data.size > 1)
		{
			ss << " " << (int)this->Imm8();
		}

		if (data.size > 2)
		{
			ss << " " << (int) High8(this->Imm16());
		}

		string result = ss.str();
		strcpy_s(output, EMU_MAX_STRINGBUF_SIZE, result.c_str());
	}
}

#pragma endregion