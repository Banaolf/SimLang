#include "silgl.h"
#include <iostream>
#include <stack>
#include <vector>
#include <string>
#include <cstdio>

#define ERR 0xF6
uint8_t state = 0;

typedef struct Runner {
    std::stack<int> stack;
    std::vector<std::string> stringtable;
    std::vector<unsigned char> code;
    FILE* file;
    std::string path;
    int ip = 0;
}Runner_t;

Runner_t rnr;

void load() {
    rnr.file = fopen(rnr.path.c_str(), "rb");
    BynaryHeader hdr;
    fread(&hdr, sizeof(BynaryHeader), 1, rnr.file);

    for (int i = 0; i < hdr.stringCount; i++) {
        std::string v;
        int c;
        while((c = fgetc(rnr.file)) != '\0' && c != EOF) v += (char)c;
        rnr.stringtable.push_back(v);
    }

    unsigned char buffer;
    while(fread(&buffer, 1, 1, rnr.file)) rnr.code.push_back(buffer);
    fclose(rnr.file);
}
void run() {
    while (rnr.ip < rnr.code.size()) {
        unsigned char opcode = rnr.code[rnr.ip++];
        switch (opcode) {
            case PUSH_INT:
            case PUSH_STRING:
            case PUSH_FLOAT:
            case POP:
            case ADDITION:
            case SUBTRACTION:
            case MULTIPLICATION:
            case DIVISION:
            case STORE_NAMED:
            case LOAD_NAMED:
            case CALL_NAMED:
            case DEF_NAMED:
            case RESTORE_NAMED:
            case JUMP:
            case RETURN:
            case ADDARG:
            case ENDARGS:
            default:
                std::cout << "Unknown opcode" << std::endl;
                state = ERR;
                return;
        }
    }
}

int main(int argc, char** argv) {
    return 0;
}