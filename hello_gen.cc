#include <string>
#include <iostream>
#include <cstring>
#define XBYAK32
#include "xbyak/xbyak.h"

class PutString: public Xbyak::CodeGenerator {
	void syscall() { db(0x0F); db(0x05); }
	void int80h() { db(0xCD); db(0x80); }
public:
	PutString(const std::string &message) {
		unsigned int *data = (unsigned int *)message.data();
		mov(dword[esp - 16], data[0]);
		mov(dword[esp - 12], data[1]);
		mov(dword[esp - 8], data[2]);
		mov(word[esp - 4], data[3]);
		mov(edx, message.length());
		dec(eax);
		mov(ebx, 1);
		jmp("@f");
		add(byte[eax], al);
		dec(eax);
		lea(esi, ptr[esp - 16]);
		mov(edi, 1);
		mov(eax, edi);
		syscall();
		mov(eax, 60);
		syscall();
L("@@");
		lea(ecx, ptr[esp - 16]);
		mov(eax, 4);
		int80h();
		xor(ebx, ebx);
		mov(eax, 1);
		int80h();
	}
};

int main(int argc, char * argv[])
{
	PutString put_string("Hello, world!\n");
	unsigned int *bin = put_string.getCode<unsigned int *>();
	size_t dwords = (put_string.getSize() + 3) / 4;
	std::string delim = "_[]={";
	for (size_t i = 0; i < dwords; ++i) {
		std::cout << delim << "0x" << std::hex << bin[i];
		delim = ",";
	}
	std::cout << "};" << std::endl;
	return 0;
}
