#ifndef EMUAPI_H
#define EMUAPI_H

#ifdef EMUAPI_EXPORTS
#define EMUAPI __declspec(dllexport)
#else
#define EMUAPI __declspec(dllimport)
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define EMU_MAX_STRINGBUF_SIZE 32

	EMUAPI typedef uint64_t EmuGenInt;

	EMUAPI typedef struct
	{
		char name[EMU_MAX_STRINGBUF_SIZE];
		EmuGenInt size;
	} EmuOpcodeData;

	EMUAPI typedef enum
	{
		statusRun = 0,
		statusHalt,
		statusUnimplementedInstruction,
	} EmuStatus;

	EMUAPI void* EmuInstantiate();

	EMUAPI void EmuDestroy(void* state);

	EMUAPI void EmuExecute(void* state);

	EMUAPI void EmuLoadMemory(void* state, void* src, EmuGenInt size, EmuGenInt offset);

	EMUAPI void EmuSetPc(void* state, EmuGenInt newPc);

	EMUAPI EmuStatus EmuGetStatus(void* state);

	EMUAPI EmuGenInt EmuOpcodeCount();

	EMUAPI EmuOpcodeData EmuGetOpcodeData(EmuGenInt opCode);

	EMUAPI void EmuDecompileNext(void* state, char output[EMU_MAX_STRINGBUF_SIZE]);

#ifdef __cplusplus
}
#endif

#endif