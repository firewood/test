#include <cstdio>
#define XBYAK32
#include "xbyak/xbyak.h"

class HelloWorld: public Xbyak::CodeGenerator {
	void syscall() { db(0x0F); db(0x05); }
	void int80h() { db(0xCD); db(0x80); }
public:
	HelloWorld() {
		mov(dword[esp - 16], 0x6c6c6548);
		mov(dword[esp - 12], 0x77202c6f);
		mov(dword[esp - 8], 0x646c726f);
		mov(word[esp - 4], 0x00000a21);
		mov(edx, 14);
		mov(edi, 1);
		dec(eax);
		mov(eax, 0x1);
		jmp("@f");
		nop();
		nop();
		dec(eax);
		lea(esi, ptr[esp - 16]);
		mov(eax, edi);
		syscall();
		mov(eax, 60);
		syscall();
L("@@");
		lea(ecx, ptr[esp - 16]);
		mov(ebx, edi);
		mov(eax, 4);
		int80h();
		mov(eax, 1);
		int80h();
	}
};

int main(int argc, char * argv[])
{
	HelloWorld hello_world;
	unsigned int *bin = hello_world.getCode<unsigned int *>();
	printf("_[]={");
	for (int i = 0; i < 22; ++i) {
		printf("%s 0x%08x", i ? "," : "", bin[i]);
	}
	printf(" };\n");
	return 0;
}
