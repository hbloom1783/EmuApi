#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include "../Emu8080/EmuApi.h"
#include "../Emu8080/Emu8080Datasheet.h"

int main(void)
{
	// Create processor
	void* processor = EmuInstantiate();

	// Load binary file
	FILE* cpuDiagFile = NULL;
	fopen_s(&cpuDiagFile, "cpudiag.bin", "rb");

	// Obtain file size
	fseek(cpuDiagFile, 0, SEEK_END);
	uint16_t fileSize = (uint16_t)ftell(cpuDiagFile);
	rewind(cpuDiagFile);

	// Copy into memory
	uint8_t* cpuDiagCode = new uint8_t[fileSize];
	fread(cpuDiagCode, 1, fileSize, cpuDiagFile);

	// Done with file
	fclose(cpuDiagFile);

	// Load BDOS
	EmuLoadMemory(processor, emu8080BDOS, 0x100, 0x0);

	// Load binary file
	EmuLoadMemory(processor, cpuDiagCode, fileSize, 0x100);

	// Move PC to binary start
	EmuSetPc(processor, 0x100);

	// Run until stop
	char decompileBuffer[EMU_MAX_STRINGBUF_SIZE];
	while (EmuGetStatus(processor) == statusRun)
	{
		//EmuDecompileNext(processor, decompileBuffer);
		//cerr << decompileBuffer << endl;
		EmuExecute(processor);
	}
}