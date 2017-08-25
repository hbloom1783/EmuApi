#pragma once

#include <string>
using std::string;

#include "Emu8080Datasheet.h"

namespace Emu8080
{
	typedef uint8_t(*InputHandler)();
	typedef void(*OutputHandler)(uint8_t);

	typedef struct
	{
		uint8_t    z : 1;
		uint8_t    s : 1;
		uint8_t    p : 1;
		uint8_t    cy : 1;
		uint8_t    ac : 1;
		uint8_t    pad : 3;
	} ConditionCodes;

	class Processor
	{
	public:
		Processor();

		void LoadMemory(void* src, uint16_t size, uint16_t offset);
		void SetPc(uint16_t newPc);

		void Execute();

		EmuStatus GetStatus();

		void SetInputHandler(uint8_t id, InputHandler callback);
		void SetOutputHandler(uint8_t id, OutputHandler callback);

		void DecompileNext(char output[EMU_MAX_STRINGBUF_SIZE]);
	private:
		uint8_t a;
		uint8_t b;
		uint8_t c;
		uint8_t d;
		uint8_t e;
		uint8_t h;
		uint8_t l;
		uint16_t sp;
		uint16_t pc;
		uint8_t memory[EMU8080_MEMORY_SIZE];
		ConditionCodes cc;

		EmuStatus status;
		uint16_t pc_incr;
		bool int_enable;

		InputHandler  input_handlers[0x100];
		OutputHandler output_handlers[0x100];

		uint8_t UpdateCnd(uint16_t newVal);

		void Add16(uint16_t other);

		void Add8(uint8_t& target, uint16_t other);
		void Add8(uint16_t other);
		void Cmp8(uint16_t other);

		void RotLeft(bool carry);
		void RotRight(bool carry);

		void Push8(uint8_t data);
		void Push16(uint16_t data);
		uint8_t Pop8();
		uint16_t Pop16();

		void Jmp(uint16_t dst);
		void Call(uint16_t dst);
		void Ret();

		uint8_t& Offset8(uint16_t offset);
		uint8_t& Offset8(uint8_t h, uint8_t l);
		uint8_t& Offset8();

		uint8_t* OpcodePtr();
		uint8_t* StackPtr();

		uint8_t Imm8();
		uint16_t Imm16();

		uint8_t& GetCC();
		
		bool IsZ();
		bool IsNZ();
		bool IsC();
		bool IsNC();
		bool IsPO();
		bool IsPE();
		bool IsP();
		bool IsM();
	};
}