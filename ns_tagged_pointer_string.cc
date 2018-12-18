#include <iostream>
#include <sstream>
#include <cstdint>
#include <cstring>

using namespace std;

// macos

static string decode_macos_tagged_pointer_string_0_7(uint64_t a) {
	int length = (a >> 4) & 15;
	string result;
	for (int i = 0; i < length; ++i) {
		result += (char)((a >> (8 + i * 8)) & 255);
	}
	return result;
}

static string decode_macos_tagged_pointer_string_8_9(uint64_t a) {
	int length = (a >> 4) & 15;
	string result;
	for (int i = 0; i < length; ++i) {
		result += "eilotrm.apdnsIc ufkMShjTRxgC4013bDNvwyUL2O856P-B79AFKEWV_zGJ/HYX"[(a >> (8 + (length - i - 1) * 6)) & 63];
	}
	return result;
}

static string decode_macos_tagged_pointer_string_10_11(uint64_t a) {
	int length = (a >> 4) & 15;
	string result;
	for (int i = 0; i < length; ++i) {
		result += "eilotrm.apdnsIc ufkMShjTRxgC4013"[(a >> (8 + (length - i - 1) * 5)) & 31];
	}
	return result;
}

static string decode_macos_tagged_pointer_string(uint64_t a) {
	if ((a & 15) != 5) {
		return "";
	}
	int length = (a >> 4) & 15;
	switch (length) {
	case 1: case 2: case 3: case 4:
	case 5: case 6: case 7:
		return decode_macos_tagged_pointer_string_0_7(a);
	case 8: case 9:
		return decode_macos_tagged_pointer_string_8_9(a);
	case 10: case 11:
		return decode_macos_tagged_pointer_string_10_11(a);
	}
	return "";
}

// ios

static string decode_ios_tagged_pointer_string_0_7(uint64_t a) {
	int length = a & 15;
	string result;
	for (int i = 0; i < length; ++i) {
		result += (char)((a >> (4 + i * 8)) & 255);
	}
	return result;
}

static string decode_ios_tagged_pointer_string_8_9(uint64_t a) {
	int length = a & 15;
	string result;
	for (int i = 0; i < length; ++i) {
		result += "eilotrm.apdnsIc ufkMShjTRxgC4013bDNvwyUL2O856P-B79AFKEWV_zGJ/HYX"[(a >> (4 + (length - i - 1) * 6)) & 63];
	}
	return result;
}

static string decode_ios_tagged_pointer_string_10_11(uint64_t a) {
	int length = a & 15;
	string result;
	for (int i = 0; i < length; ++i) {
		result += "eilotrm.apdnsIc ufkMShjTRxgC4013"[(a >> (4 + (length - i - 1) * 5)) & 31];
	}
	return result;
}

static string decode_ios_tagged_pointer_string(uint64_t a) {
	if ((a & 0xa000000000000000) != 0xa000000000000000) {
		return "";
	}
	int length = a & 15;
	switch (length) {
	case 1: case 2: case 3: case 4:
	case 5: case 6: case 7:
		return decode_ios_tagged_pointer_string_0_7(a);
	case 8: case 9:
		return decode_ios_tagged_pointer_string_8_9(a);
	case 10: case 11:
		return decode_ios_tagged_pointer_string_10_11(a);
	}
	return "";
}

// test

static bool test_decode_macos_tagged_pointer_string(uint64_t a, const string &s) {
	string result = decode_macos_tagged_pointer_string(a);
	printf("0x%016llx -> %s\n", a, result.c_str());
	return s == result;
}

static bool test_decode_ios_tagged_pointer_string(uint64_t a, const string &s) {
	string result = decode_ios_tagged_pointer_string(a);
	printf("0x%016llx -> %s\n", a, result.c_str());
	return s == result;
}

int main(int argc, char *argv[], char *envp[]) {
	test_decode_macos_tagged_pointer_string(0x0000000000006115, "a");
	test_decode_macos_tagged_pointer_string(0x0000000063626135, "abc");
	test_decode_macos_tagged_pointer_string(0x6766656463626175, "abcdefg");
	test_decode_macos_tagged_pointer_string(0x0022038a01169585, "abcdefgh");
	test_decode_macos_tagged_pointer_string(0x0880e28045a54195, "abcdefghi");
	test_decode_macos_tagged_pointer_string(0x39408eaa1b4846b5, "cdefghijklm");

	test_decode_ios_tagged_pointer_string(0xa000000000000611, "a");
	test_decode_ios_tagged_pointer_string(0xa0022038a0116958, "abcdefgh");
	test_decode_ios_tagged_pointer_string(0xa0880e28045a5419, "abcdefghi");
	test_decode_ios_tagged_pointer_string(0xa02a01087500843a, "hellohello");

//s	NSTaggedPointerString *	@"abcdefghi"	0xc94c01116118ff83

// p [(NSObject *)[@"abcdefgh" mutableCopy] copy]

	return 0;
}
