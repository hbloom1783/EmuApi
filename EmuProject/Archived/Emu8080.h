#ifndef EMU8080_H
#define EMU8080_H

#include <stdint.h>

#include "Emu8080Opcodes.h"

#define EMU_8080_MEMORY_SIZE 0x10000

typedef uint8_t(*InputHandler)();
typedef void(*OutputHandler)(uint8_t);

typedef struct ConditionCodes
{
	uint8_t    z : 1;
	uint8_t    s : 1;
	uint8_t    p : 1;
	uint8_t    cy : 1;
	uint8_t    ac : 1;
	uint8_t    pad : 3;
} ConditionCodes;

typedef enum
{
	stateRun = 0,
	stateHalt,
	stateUnimplementedInstruction,
} emuState;

typedef struct State8080
{
	uint8_t    a;
	uint8_t    b;
	uint8_t    c;
	uint8_t    d;
	uint8_t    e;
	uint8_t    h;
	uint8_t    l;
	uint16_t   sp;
	uint16_t   pc;
	uint8_t    memory[EMU_8080_MEMORY_SIZE];
	struct     ConditionCodes      cc;
	uint8_t    int_enable;

	emuState   state;
	uint16_t   pc_incr;

	InputHandler  input_handlers[0x100];
	OutputHandler output_handlers[0x100];
} State8080;

void Emulate8080Op(State8080* state);

int OpLen8080(Emu8080Opcode opcode);

#endif