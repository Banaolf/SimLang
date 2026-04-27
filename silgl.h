#ifndef __SIL_GLOBALS //C-Style bcuz why not
#define __SIL_GLOBALS 1

enum opcodes {
	PUSH_INT = 0x00, //Booleans are treated as integers
	PUSH_STRING = 0x01, //Raw strings and strings are the same to the compiler.
	PUSH_FLOAT = 0x02,
	POP = 0x03,
	ADDITION = 0x04,
	SUBTRACTION = 0x05,
	MULTIPLICATION = 0x06,
	DIVISION = 0x07,
	STORE_NAMED = 0x08,
	LOAD_NAMED = 0x09,
	CALL_NAMED = 0x0A,
	DEF_NAMED = 0x0B,
	RESTORE_NAMED = 0x0C,
	JUMP = 0x0D,
	RETURN = 0x0E,
	ADDARG = 0x0F,
	ENDARGS = 0x10,
};
#include <cstdint>
#define VERSION "SIL1"
#define VERNUM 1
#pragma pack(push, 1)
struct CompilerConfigs {
	uint8_t optimisation = 0x00;
	uint8_t warnings = 0x01;
};
struct BynaryHeader {
	char magic[5] = "SIL!";
	int version = VERNUM;
	int minversion = VERNUM;
	CompilerConfigs flags;
	uint32_t stringCount;
	uint32_t codeSize;
};
#pragma pack(pop)

#endif