#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "Emu8080.h"
#include "Emu8080Opcodes.h"

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

uint8_t LowNibble(uint8_t x, uint8_t y)
{
	return (x & 0xF) + (y & 0xF);
}

uint8_t Cnd8080(State8080* state, uint16_t value, uint8_t lownibble)
{
	state->cc.z = ((value & 0xff) == 0);
	state->cc.s = ((value & 0x80) != 0);
	state->cc.p = Parity(value & 0xff);
	state->cc.cy = (value > 0xff);
	state->cc.ac = (lownibble > 0xf);
	return value & 0xff;
}

uint8_t* Offset8080(State8080* state)
{
	return state->memory +  ((state->h << 8) | (state->l));
}

void Add8080(State8080* state, uint16_t other)
{
	state->a = Cnd8080(state, (uint16_t) state->a + other, LowNibble(state->a, other));
}

void Sub8080(State8080* state, uint16_t other)
{
	state->a = Cnd8080(state, (uint16_t) state->a - other, LowNibble(state->a, -other));
}

void Inr8080(State8080* state, uint8_t* target)
{
	(*target)++;
	Cnd8080(state, *target, LowNibble(*target, 1));
}

void Dcr8080(State8080* state, uint8_t* target)
{
	(*target)--;
	Cnd8080(state, *target, LowNibble(*target, -1));
}

void Ana8080(State8080* state, uint16_t other)
{
	state->a = Cnd8080(state, (uint16_t) state->a & other, 0);
}

void Xra8080(State8080* state, uint16_t other)
{
	state->a = Cnd8080(state, (uint16_t) state->a ^ other, 0);
}

void Ora8080(State8080* state, uint16_t other)
{
	state->a = Cnd8080(state, (uint16_t) state->a | other, 0);
}

void Cmp8080(State8080* state, uint16_t other)
{
	Cnd8080(state, (uint16_t) state->a - other, LowNibble(state->a, -other));
}

uint16_t Compose16(uint8_t h, uint8_t l)
{
	return (h << 8) | l;
}

uint8_t High8(uint16_t data)
{
	return (data >> 8) & 0xFF;
}

uint8_t Low8(uint16_t data)
{
	return data & 0xFF;
}

void Inx8080(State8080* state, uint8_t* h, uint8_t* l)
{
	uint16_t comp = Compose16(*h, *l);

	comp += 1;

	*h = High8(comp);
	*l = Low8(comp);
}

void Dcx8080(State8080* state, uint8_t* h, uint8_t* l)
{
	uint16_t comp = Compose16(*h, *l);

	comp -= 1;

	*h = High8(comp);
	*l = Low8(comp);
}

void Dad8080(State8080* state, uint8_t h, uint8_t l)
{
	uint16_t comp = Compose16(h, l);

	uint16_t hl = Compose16(state->h, state->l);

	uint32_t answer = (uint32_t)hl + (uint32_t)comp;
	state->cc.cy = (answer > 0xffff);
	hl = answer & 0xffff;

	state->h = High8(hl);
	state->l = Low8(hl);
}

int IsZ(State8080* state)
{
	return state->cc.z == 1;
}

int IsNZ(State8080* state)
{
	return state->cc.z == 0;
}

int IsC(State8080* state)
{
	return state->cc.cy == 1;
}

int IsNC(State8080* state)
{
	return state->cc.cy == 0;
}

int IsPO(State8080* state)
{
	return state->cc.p == 0;
}

int IsPE(State8080* state)
{
	return state->cc.p == 1;
}

int IsP(State8080* state)
{
	return state->cc.s == 0;
}

int IsM(State8080* state)
{
	return state->cc.s == 1;
}

void Push8(State8080* state, uint8_t data)
{
	state->memory[--state->sp] = data;
}

uint8_t Pop8(State8080* state)
{
	return state->memory[state->sp++];
}

void Push16(State8080* state, uint16_t data)
{
	Push8(state, High8(data));
	Push8(state, Low8(data));
}

uint16_t Pop16(State8080* state)
{
	uint8_t l = Pop8(state);
	uint8_t h = Pop8(state);
	return Compose16(h, l);
}

void Jmp8080(State8080* state, uint16_t addr)
{
	state->pc = addr;
	state->pc_incr = 0;
}

void Call8080(State8080* state, uint16_t addr)
{
	Push16(state, state->pc + 3);
	Jmp8080(state, addr);
}

void Ret8080(State8080* state)
{
	state->pc = Pop16(state);
	state->pc_incr = 0;
}

void RRC8080(State8080* state)
{
	uint8_t x = state->a;
	state->a = ((x & 0x01) << 7) | (x >> 1);
	state->cc.cy = (0 != (x & 0x01));
}

void RLC8080(State8080* state)
{
	uint8_t x = state->a;
	state->a = ((x & 0x80) >> 7) | (x << 1);
	state->cc.cy = (0 != (x & 0x80));
}

void RAR8080(State8080* state)
{
	uint8_t x = state->a;
	state->a = (state->cc.cy << 7) | (x >> 1);
	state->cc.cy = (0 != (x & 0x01));
}

void RAL8080(State8080* state)
{
	uint8_t x = state->a;
	state->a = (state->cc.cy) | (x << 1);
	state->cc.cy = (0 != (x & 0x80));
}

void Swap8(uint8_t* left, uint8_t* right)
{
	uint8_t swap = *left;
	*left = *right;
	*right = swap;
}

uint8_t GetCC(State8080* state)
{
	return *((int*) &(state->cc));
}

void SetCC(State8080* state, uint8_t val)
{
	*((int*) &(state->cc)) = val;
}

void Emulate8080Op(State8080* state)
{
	if (state->state == stateRun)
	{
		uint8_t *opcode = &state->memory[state->pc];
		state->pc_incr = 1;

		/*switch (OpLen8080(*opcode))
		{
		case 3:
			printf(
				"[%04X] %02X %02X %02X\n",
				state->pc,
				state->memory[state->pc],
				state->memory[state->pc + 1],
				state->memory[state->pc + 2]);
			break;

		case 2:
			printf(
				"[%04X] %02X %02X\n",
				state->pc,
				state->memory[state->pc],
				state->memory[state->pc + 1]);
			break;

		case 1:
			printf(
				"[%04X] %02X\n",
				state->pc,
				state->memory[state->pc]);
			break;

		case 0:
		default:
			printf(
				"[%04X] %02X %02X %02X\n",
				state->pc );
			break;
		}*/

		switch (*opcode)
		{

#pragma region Store / Load

		case op8080_STA: state->memory[Compose16(opcode[2], opcode[1])] = state->a; state->pc_incr = 3; break;

		case op8080_LDA: state->a = state->memory[Compose16(opcode[2], opcode[1])]; state->pc_incr = 3; break;

		case op8080_STAX_B: state->memory[Compose16(state->b, state->c)] = state->a; break;
		case op8080_STAX_D: state->memory[Compose16(state->d, state->e)] = state->a; break;

		case op8080_LDAX_B: state->a = state->memory[Compose16(state->b, state->c)]; break;
		case op8080_LDAX_D: state->a = state->memory[Compose16(state->d, state->e)]; break;

		case op8080_SHLD:
			state->memory[Compose16(opcode[2], opcode[1]) + 1] = state->h;
			state->memory[Compose16(opcode[2], opcode[1])] = state->l;
			state->pc_incr = 3;
			break;

		case op8080_LHLD:
			state->h = state->memory[Compose16(opcode[2], opcode[1]) + 1];
			state->l = state->memory[Compose16(opcode[2], opcode[1])];
			state->pc_incr = 3;
			break;

		case op8080_LXI_B:
			state->c = opcode[1];
			state->b = opcode[2];
			state->pc_incr = 3;
			break;

		case op8080_LXI_D:
			state->e = opcode[1];
			state->d = opcode[2];
			state->pc_incr = 3;
			break;

		case op8080_LXI_H:
			state->l = opcode[1];
			state->h = opcode[2];
			state->pc_incr = 3;
			break;

		case op8080_LXI_SP:
			state->sp = Compose16(opcode[2], opcode[1]);
			state->pc_incr = 3;
			break;

#pragma endregion

#pragma region MOV

			// MOV B,
		case op8080_MOV_B_B: state->b = state->b; break;
		case op8080_MOV_B_C: state->b = state->c; break;
		case op8080_MOV_B_D: state->b = state->d; break;
		case op8080_MOV_B_E: state->b = state->e; break;
		case op8080_MOV_B_H: state->b = state->h; break;
		case op8080_MOV_B_L: state->b = state->l; break;
		case op8080_MOV_B_M: state->b = *Offset8080(state); break;
		case op8080_MOV_B_A: state->b = state->a; break;

			// MOV C,
		case op8080_MOV_C_B: state->c = state->b; break;
		case op8080_MOV_C_C: state->c = state->c; break;
		case op8080_MOV_C_D: state->c = state->d; break;
		case op8080_MOV_C_E: state->c = state->e; break;
		case op8080_MOV_C_H: state->c = state->h; break;
		case op8080_MOV_C_L: state->c = state->l; break;
		case op8080_MOV_C_M: state->c = *Offset8080(state); break;
		case op8080_MOV_C_A: state->c = state->a; break;

			// MOV D,
		case op8080_MOV_D_B: state->d = state->b; break;
		case op8080_MOV_D_C: state->d = state->c; break;
		case op8080_MOV_D_D: state->d = state->d; break;
		case op8080_MOV_D_E: state->d = state->e; break;
		case op8080_MOV_D_H: state->d = state->h; break;
		case op8080_MOV_D_L: state->d = state->l; break;
		case op8080_MOV_D_M: state->d = *Offset8080(state); break;
		case op8080_MOV_D_A: state->d = state->a; break;

			// MOV E,
		case op8080_MOV_E_B: state->e = state->b; break;
		case op8080_MOV_E_C: state->e = state->c; break;
		case op8080_MOV_E_D: state->e = state->d; break;
		case op8080_MOV_E_E: state->e = state->e; break;
		case op8080_MOV_E_H: state->e = state->h; break;
		case op8080_MOV_E_L: state->e = state->l; break;
		case op8080_MOV_E_M: state->e = *Offset8080(state); break;
		case op8080_MOV_E_A: state->e = state->a; break;

			// MOV H,
		case op8080_MOV_H_B: state->h = state->b; break;
		case op8080_MOV_H_C: state->h = state->c; break;
		case op8080_MOV_H_D: state->h = state->d; break;
		case op8080_MOV_H_E: state->h = state->e; break;
		case op8080_MOV_H_H: state->h = state->h; break;
		case op8080_MOV_H_L: state->h = state->l; break;
		case op8080_MOV_H_M: state->h = *Offset8080(state); break;
		case op8080_MOV_H_A: state->h = state->a; break;

			// MOV L,
		case op8080_MOV_L_B: state->l = state->b; break;
		case op8080_MOV_L_C: state->l = state->c; break;
		case op8080_MOV_L_D: state->l = state->d; break;
		case op8080_MOV_L_E: state->l = state->e; break;
		case op8080_MOV_L_H: state->l = state->h; break;
		case op8080_MOV_L_L: state->l = state->l; break;
		case op8080_MOV_L_M: state->l = *Offset8080(state); break;
		case op8080_MOV_L_A: state->l = state->a; break;

			// MOV M,
		case op8080_MOV_M_B: *Offset8080(state) = state->b; break;
		case op8080_MOV_M_C: *Offset8080(state) = state->c; break;
		case op8080_MOV_M_D: *Offset8080(state) = state->d; break;
		case op8080_MOV_M_E: *Offset8080(state) = state->e; break;
		case op8080_MOV_M_H: *Offset8080(state) = state->h; break;
		case op8080_MOV_M_L: *Offset8080(state) = state->l; break;
		case op8080_MOV_M_A: *Offset8080(state) = state->a; break;

			// MOV A,
		case op8080_MOV_A_B: state->a = state->b; break;
		case op8080_MOV_A_C: state->a = state->c; break;
		case op8080_MOV_A_D: state->a = state->d; break;
		case op8080_MOV_A_E: state->a = state->e; break;
		case op8080_MOV_A_H: state->a = state->h; break;
		case op8080_MOV_A_L: state->a = state->l; break;
		case op8080_MOV_A_M: state->a = *Offset8080(state); break;
		case op8080_MOV_A_A: state->a = state->a; break;

#pragma endregion

#pragma region MVI

		case op8080_MVI_B: state->b = opcode[1]; state->pc_incr = 2; break;
		case op8080_MVI_C: state->c = opcode[1]; state->pc_incr = 2; break;
		case op8080_MVI_D: state->d = opcode[1]; state->pc_incr = 2; break;
		case op8080_MVI_E: state->e = opcode[1]; state->pc_incr = 2; break;
		case op8080_MVI_H: state->h = opcode[1]; state->pc_incr = 2; break;
		case op8080_MVI_L: state->l = opcode[1]; state->pc_incr = 2; break;
		case op8080_MVI_M: *Offset8080(state) = opcode[1]; state->pc_incr = 2; break;
		case op8080_MVI_A: state->a = opcode[1]; state->pc_incr = 2; break;

#pragma endregion

#pragma region Addition

			// INR
		case op8080_INR_B: Inr8080(state, &(state->b)); break;
		case op8080_INR_C: Inr8080(state, &(state->c)); break;
		case op8080_INR_D: Inr8080(state, &(state->d)); break;
		case op8080_INR_E: Inr8080(state, &(state->e)); break;
		case op8080_INR_H: Inr8080(state, &(state->h)); break;
		case op8080_INR_L: Inr8080(state, &(state->l)); break;
		case op8080_INR_M: Inr8080(state, Offset8080(state)); break;
		case op8080_INR_A: Inr8080(state, &(state->a)); break;

			// ADD
		case op8080_ADD_B: Add8080(state, state->b); break;
		case op8080_ADD_C: Add8080(state, state->c); break;
		case op8080_ADD_D: Add8080(state, state->d); break;
		case op8080_ADD_E: Add8080(state, state->e); break;
		case op8080_ADD_H: Add8080(state, state->h); break;
		case op8080_ADD_L: Add8080(state, state->l); break;
		case op8080_ADD_M: Add8080(state, *Offset8080(state)); break;
		case op8080_ADD_A: Add8080(state, state->a); break;

			// ADI
		case op8080_ADI: Add8080(state, opcode[1]); state->pc_incr = 2; break;

			// ADC
		case op8080_ADC_B: Add8080(state, state->cc.cy + state->b); break;
		case op8080_ADC_C: Add8080(state, state->cc.cy + state->c); break;
		case op8080_ADC_D: Add8080(state, state->cc.cy + state->d); break;
		case op8080_ADC_E: Add8080(state, state->cc.cy + state->e); break;
		case op8080_ADC_H: Add8080(state, state->cc.cy + state->h); break;
		case op8080_ADC_L: Add8080(state, state->cc.cy + state->l); break;
		case op8080_ADC_M: Add8080(state, state->cc.cy + *Offset8080(state)); break;
		case op8080_ADC_A: Add8080(state, state->cc.cy + state->a); break;

			// ACI
		case op8080_ACI: Add8080(state, state->cc.cy + opcode[1]); state->pc_incr = 2; break;

#pragma endregion

#pragma region Subtraction

			// DCR
		case op8080_DCR_B: Dcr8080(state, &(state->b)); break;
		case op8080_DCR_C: Dcr8080(state, &(state->c)); break;
		case op8080_DCR_D: Dcr8080(state, &(state->d)); break;
		case op8080_DCR_E: Dcr8080(state, &(state->e)); break;
		case op8080_DCR_H: Dcr8080(state, &(state->h)); break;
		case op8080_DCR_L: Dcr8080(state, &(state->l)); break;
		case op8080_DCR_M: Dcr8080(state, Offset8080(state)); break;
		case op8080_DCR_A: Dcr8080(state, &(state->a)); break;

			// SUB
		case op8080_SUB_B: Sub8080(state, state->b); break;
		case op8080_SUB_C: Sub8080(state, state->c); break;
		case op8080_SUB_D: Sub8080(state, state->d); break;
		case op8080_SUB_E: Sub8080(state, state->e); break;
		case op8080_SUB_H: Sub8080(state, state->h); break;
		case op8080_SUB_L: Sub8080(state, state->l); break;
		case op8080_SUB_M: Sub8080(state, *Offset8080(state)); break;
		case op8080_SUB_A: Sub8080(state, state->a); break;

			// SUI
		case op8080_SUI: Sub8080(state, opcode[1]); state->pc_incr = 2; break;

			// SBB
		case op8080_SBB_B: Sub8080(state, state->cc.cy + state->b); break;
		case op8080_SBB_C: Sub8080(state, state->cc.cy + state->c); break;
		case op8080_SBB_D: Sub8080(state, state->cc.cy + state->d); break;
		case op8080_SBB_E: Sub8080(state, state->cc.cy + state->e); break;
		case op8080_SBB_H: Sub8080(state, state->cc.cy + state->h); break;
		case op8080_SBB_L: Sub8080(state, state->cc.cy + state->l); break;
		case op8080_SBB_M: Sub8080(state, state->cc.cy + *Offset8080(state)); break;
		case op8080_SBB_A: Sub8080(state, state->cc.cy + state->a); break;

			// SBI
		case op8080_SBI: Sub8080(state, state->cc.cy + opcode[1]); state->pc_incr = 2; break;

			// CMP
		case op8080_CMP_B: Cmp8080(state, state->b); break;
		case op8080_CMP_C: Cmp8080(state, state->c); break;
		case op8080_CMP_D: Cmp8080(state, state->d); break;
		case op8080_CMP_E: Cmp8080(state, state->e); break;
		case op8080_CMP_H: Cmp8080(state, state->h); break;
		case op8080_CMP_L: Cmp8080(state, state->l); break;
		case op8080_CMP_M: Cmp8080(state, *Offset8080(state)); break;
		case op8080_CMP_A: Cmp8080(state, state->a); break;

			// SBI
		case op8080_CPI: Cmp8080(state, opcode[1]); state->pc_incr = 2; break;

#pragma endregion

#pragma region Boolean Logic

			// ANA
		case op8080_ANA_B: Ana8080(state, state->b); break;
		case op8080_ANA_C: Ana8080(state, state->c); break;
		case op8080_ANA_D: Ana8080(state, state->d); break;
		case op8080_ANA_E: Ana8080(state, state->e); break;
		case op8080_ANA_H: Ana8080(state, state->h); break;
		case op8080_ANA_L: Ana8080(state, state->l); break;
		case op8080_ANA_M: Ana8080(state, *Offset8080(state)); break;
		case op8080_ANA_A: Ana8080(state, state->a); break;

		case op8080_ANI: Ana8080(state, opcode[1]); state->pc_incr = 2; break;

			// XRA
		case op8080_XRA_B: Xra8080(state, state->b); break;
		case op8080_XRA_C: Xra8080(state, state->c); break;
		case op8080_XRA_D: Xra8080(state, state->d); break;
		case op8080_XRA_E: Xra8080(state, state->e); break;
		case op8080_XRA_H: Xra8080(state, state->h); break;
		case op8080_XRA_L: Xra8080(state, state->l); break;
		case op8080_XRA_M: Xra8080(state, *Offset8080(state)); break;
		case op8080_XRA_A: Xra8080(state, state->a); break;

		case op8080_XRI: Xra8080(state, opcode[1]); state->pc_incr = 2; break;

			// ORA
		case op8080_ORA_B: Ora8080(state, state->b); break;
		case op8080_ORA_C: Ora8080(state, state->c); break;
		case op8080_ORA_D: Ora8080(state, state->d); break;
		case op8080_ORA_E: Ora8080(state, state->e); break;
		case op8080_ORA_H: Ora8080(state, state->h); break;
		case op8080_ORA_L: Ora8080(state, state->l); break;
		case op8080_ORA_M: Ora8080(state, *Offset8080(state)); break;
		case op8080_ORA_A: Ora8080(state, state->a); break;

		case op8080_ORI: Ora8080(state, opcode[1]); state->pc_incr = 2; break;

			// CMA
		case op8080_CMA: state->a = (~state->a) & 0xFF; break;

#pragma endregion

#pragma region 16-bit Arithmetic

			// INX
		case op8080_INX_B: Inx8080(state, &(state->b), &(state->c)); break; // B
		case op8080_INX_D: Inx8080(state, &(state->d), &(state->e)); break; // D
		case op8080_INX_H: Inx8080(state, &(state->h), &(state->l)); break; // H
		case op8080_INX_SP: state->sp += 1; break; // SP

		// DCX
		case op8080_DCX_B: Dcx8080(state, &(state->b), &(state->c)); break; // B
		case op8080_DCX_D: Dcx8080(state, &(state->d), &(state->e)); break; // D
		case op8080_DCX_H: Dcx8080(state, &(state->h), &(state->l)); break; // H
		case op8080_DCX_SP: state->sp -= 1; break; // SP

		// DAD
		case op8080_DAD_B: Dad8080(state, state->b, state->c); break;
		case op8080_DAD_D: Dad8080(state, state->d, state->e); break;
		case op8080_DAD_H: Dad8080(state, state->h, state->l); break;
		case op8080_DAD_SP: Dad8080(state, High8(state->sp), Low8(state->sp)); break;

#pragma endregion

#pragma region Stack

		case op8080_PUSH_B: Push16(state, Compose16(state->b, state->c)); break;
		case op8080_PUSH_D: Push16(state, Compose16(state->d, state->e)); break;
		case op8080_PUSH_H: Push16(state, Compose16(state->h, state->l)); break;
		case op8080_PUSH_PSW: Push16(state, Compose16(state->a, GetCC(state))); break;

		case op8080_POP_B:
		{
			uint16_t pop = Pop16(state);
			state->b = High8(pop);
			state->c = Low8(pop);
		} break;
		case op8080_POP_D:
		{
			uint16_t pop = Pop16(state);
			state->d = High8(pop);
			state->e = Low8(pop);
		} break;
		case op8080_POP_H:
		{
			uint16_t pop = Pop16(state);
			state->h = High8(pop);
			state->l = Low8(pop);
		} break;
		case op8080_POP_PSW:
		{
			uint16_t pop = Pop16(state);
			state->a = High8(pop);
			SetCC(state, Low8(pop));
		} break;

#pragma endregion

#pragma region Branching

		// JMP
		case op8080_JMP: Jmp8080(state, Compose16(opcode[2], opcode[1])); break;
		case op8080_JNZ: if (IsNZ(state)) Jmp8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_JZ:  if (IsZ(state)) Jmp8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_JNC: if (IsNC(state)) Jmp8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_JC:  if (IsC(state)) Jmp8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_JPO: if (IsPO(state)) Jmp8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_JPE: if (IsPE(state)) Jmp8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_JP:  if (IsP(state)) Jmp8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_JM:  if (IsM(state)) Jmp8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;

			// CALL
		case op8080_CALL: Call8080(state, Compose16(opcode[2], opcode[1])); break;
		case op8080_CNZ:  if (IsNZ(state)) Call8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_CZ:   if (IsZ(state)) Call8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_CNC:  if (IsNC(state)) Call8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_CC:   if (IsC(state)) Call8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_CPO:  if (IsPO(state)) Call8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_CPE:  if (IsPE(state)) Call8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_CP:   if (IsP(state)) Call8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;
		case op8080_CM:   if (IsM(state)) Call8080(state, Compose16(opcode[2], opcode[1])); else state->pc_incr = 3; break;

			// RET
		case op8080_RET: Ret8080(state); break;
		case op8080_RNZ: if (IsNZ(state)) Ret8080(state); break;
		case op8080_RZ:  if (IsZ(state)) Ret8080(state); break;
		case op8080_RNC: if (IsNC(state)) Ret8080(state); break;
		case op8080_RC:  if (IsC(state)) Ret8080(state); break;
		case op8080_RPO: if (IsPO(state)) Ret8080(state); break;
		case op8080_RPE: if (IsPE(state)) Ret8080(state); break;
		case op8080_RP:  if (IsP(state)) Ret8080(state); break;
		case op8080_RM:  if (IsM(state)) Ret8080(state); break;

			// RST
		case op8080_RST_0: Call8080(state, 0x00); break;
		case op8080_RST_1: Call8080(state, 0x08); break;
		case op8080_RST_2: Call8080(state, 0x10); break;
		case op8080_RST_3: Call8080(state, 0x18); break;
		case op8080_RST_4: Call8080(state, 0x20); break;
		case op8080_RST_5: Call8080(state, 0x28); break;
		case op8080_RST_6: Call8080(state, 0x30); break;
		case op8080_RST_7: Call8080(state, 0x38); break;

#pragma endregion

#pragma region I/O

		case op8080_OUT:
			if (state->output_handlers[opcode[1]] != 0)
				state->output_handlers[opcode[1]](state->a);
			state->pc_incr = 2;
			//putchar(state->a);
			//printf("OUT %d: %02X (%3d, %c)\n", opcode[1], state->a, state->a, state->a);
			break;

		case op8080_IN:
			if (state->input_handlers[opcode[1]] != 0)
				state->a = state->input_handlers[opcode[1]]();
			state->pc_incr = 2;
			break;

#pragma endregion

#pragma region Rotation

		case op8080_RRC: RRC8080(state); break;

		case op8080_RAR: RAR8080(state); break;

		case op8080_RLC: RLC8080(state); break;

		case op8080_RAL: RAL8080(state); break;

		case op8080_XCHG: Swap8(&(state->h), &(state->d)); Swap8(&(state->l), &(state->e)); break;

		case op8080_XTHL: Swap8(&(state->h), state->memory + state->sp + 1); Swap8(&(state->l), state->memory + state->sp); break;

#pragma endregion

			// STC
		case op8080_STC: state->cc.cy = 1; break;

			// CMC
		case op8080_CMC: state->cc.cy = 1 - state->cc.cy; break;

			// NOP
		case op8080_NOP: break;

			// HLT
		case op8080_HLT:
			state->pc_incr = 0;
			state->state = stateHalt;
			break;

		case op8080_DI:
			state->int_enable = 0;
			break;

		case op8080_EI:
			state->int_enable = 1;
			break;

		case op8080_DAA:
		{
			uint16_t val = state->a;

			if (((val & 0x0F) > 0x09) || (state->cc.ac == 1))
			{
				val += 0x06;
			}

			if (((val & 0xF0) > 0x90) || (state->cc.cy == 1))
			{
				val += 0x60;
				state->cc.cy = 1;
			}
			else
			{
				state->cc.cy = 0;
			}

			state->a = val & 0xFF;
		} break;

		case op8080_SPHL: state->sp = Compose16(state->h, state->l); break;
		case op8080_PCHL: Jmp8080(state, Compose16(state->h, state->l)); break;

		default:
			state->pc_incr = 0;
			state->state = stateUnimplementedInstruction;
			break;
		}

		state->pc += state->pc_incr;
	}
}

int OpLen8080(Emu8080Opcode opcode)
{
	switch (opcode)
	{
	case 0x00:
	case 0x02:
	case 0x03:
	case 0x04:
	case 0x05:
	case 0x07:
	case 0x09:
	case 0x0a:
	case 0x0b:
	case 0x0c:
	case 0x0d:
	case 0x0f:
	case 0x12:
	case 0x13:
	case 0x14:
	case 0x15:
	case 0x17:
	case 0x19:
	case 0x1a:
	case 0x1b:
	case 0x1c:
	case 0x1d:
	case 0x1f:
	case 0x20:
	case 0x23:
	case 0x24:
	case 0x25:
	case 0x27:
	case 0x29:
	case 0x2b:
	case 0x2c:
	case 0x2d:
	case 0x2f:
	case 0x30:
	case 0x33:
	case 0x34:
	case 0x35:
	case 0x37:
	case 0x39:
	case 0x3b:
	case 0x3c:
	case 0x3d:
	case 0x3f:
	case 0x40:
	case 0x41:
	case 0x42:
	case 0x43:
	case 0x44:
	case 0x45:
	case 0x46:
	case 0x47:
	case 0x48:
	case 0x49:
	case 0x4a:
	case 0x4b:
	case 0x4c:
	case 0x4d:
	case 0x4e:
	case 0x4f:
	case 0x50:
	case 0x51:
	case 0x52:
	case 0x53:
	case 0x54:
	case 0x55:
	case 0x56:
	case 0x57:
	case 0x58:
	case 0x59:
	case 0x5a:
	case 0x5b:
	case 0x5c:
	case 0x5d:
	case 0x5e:
	case 0x5f:
	case 0x60:
	case 0x61:
	case 0x62:
	case 0x63:
	case 0x64:
	case 0x65:
	case 0x66:
	case 0x67:
	case 0x68:
	case 0x69:
	case 0x6a:
	case 0x6b:
	case 0x6c:
	case 0x6d:
	case 0x6e:
	case 0x6f:
	case 0x70:
	case 0x71:
	case 0x72:
	case 0x73:
	case 0x74:
	case 0x75:
	case 0x76:
	case 0x77:
	case 0x78:
	case 0x79:
	case 0x7a:
	case 0x7b:
	case 0x7c:
	case 0x7d:
	case 0x7e:
	case 0x7f:
	case 0x80:
	case 0x81:
	case 0x82:
	case 0x83:
	case 0x84:
	case 0x85:
	case 0x86:
	case 0x87:
	case 0x88:
	case 0x89:
	case 0x8a:
	case 0x8b:
	case 0x8c:
	case 0x8d:
	case 0x8e:
	case 0x8f:
	case 0x90:
	case 0x91:
	case 0x92:
	case 0x93:
	case 0x94:
	case 0x95:
	case 0x96:
	case 0x97:
	case 0x98:
	case 0x99:
	case 0x9a:
	case 0x9b:
	case 0x9c:
	case 0x9d:
	case 0x9e:
	case 0x9f:
	case 0xa0:
	case 0xa1:
	case 0xa2:
	case 0xa3:
	case 0xa4:
	case 0xa5:
	case 0xa6:
	case 0xa7:
	case 0xa8:
	case 0xa9:
	case 0xaa:
	case 0xab:
	case 0xac:
	case 0xad:
	case 0xae:
	case 0xaf:
	case 0xb0:
	case 0xb1:
	case 0xb2:
	case 0xb3:
	case 0xb4:
	case 0xb5:
	case 0xb6:
	case 0xb7:
	case 0xb8:
	case 0xb9:
	case 0xba:
	case 0xbb:
	case 0xbc:
	case 0xbd:
	case 0xbe:
	case 0xbf:
	case 0xc0:
	case 0xc1:
	case 0xc5:
	case 0xc7:
	case 0xc8:
	case 0xc9:
	case 0xcf:
	case 0xd0:
	case 0xd1:
	case 0xd5:
	case 0xd7:
	case 0xd8:
	case 0xdf:
	case 0xe0:
	case 0xe1:
	case 0xe3:
	case 0xe5:
	case 0xe7:
	case 0xe8:
	case 0xe9:
	case 0xeb:
	case 0xef:
	case 0xf0:
	case 0xf1:
	case 0xf3:
	case 0xf5:
	case 0xf7:
	case 0xf8:
	case 0xf9:
	case 0xfb:
	case 0xff:
		return 1;
		break;

	case 0x06:
	case 0x0e:
	case 0x16:
	case 0x1e:
	case 0x26:
	case 0x2e:
	case 0x36:
	case 0x3e:
	case 0xc6:
	case 0xce:
	case 0xd3:
	case 0xd6:
	case 0xdb:
	case 0xde:
	case 0xe6:
	case 0xee:
	case 0xf6:
	case 0xfe:
		return 2;
		break;

	case 0x01:
	case 0x11:
	case 0x21:
	case 0x22:
	case 0x2a:
	case 0x31:
	case 0x32:
	case 0x3a:
	case 0xc2:
	case 0xc3:
	case 0xc4:
	case 0xca:
	case 0xcc:
	case 0xcd:
	case 0xd2:
	case 0xd4:
	case 0xda:
	case 0xdc:
	case 0xe2:
	case 0xe4:
	case 0xea:
	case 0xec:
	case 0xf2:
	case 0xf4:
	case 0xfa:
	case 0xfc:
		return 3;
		break;

	case 0x08:
	case 0x10:
	case 0x18:
	case 0x28:
	case 0x38:
	case 0xcb:
	case 0xd9:
	case 0xdd:
	case 0xed:
	case 0xfd:
	default:
		return 0;
		break;
	}
}