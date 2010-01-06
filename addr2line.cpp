
// add2line.cpp

#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string>
#include <vector>

// The size of the DW_LNE_set_address operand depends on the target machine.
typedef char *addr_t;

typedef signed char sbyte;
typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef signed long slong;
typedef unsigned long ulong;

class CBinaryFile {
	enum {
		DW_LNS_extended_op = 0,
		DW_LNS_copy = 1,
		DW_LNS_advance_pc = 2,
		DW_LNS_advance_line = 3,
		DW_LNS_set_file = 4,
		DW_LNS_set_column = 5,
		DW_LNS_negate_stmt = 6,
		DW_LNS_set_basic_block = 7,
		DW_LNS_const_add_pc = 8,
		DW_LNS_fixed_advance_pc = 9,
		DW_LNS_set_prologue_end = 10,
		DW_LNS_set_epilogue_begin = 11,
		DW_LNS_set_isa = 12,

		DW_LNE_end_sequence = 1,
		DW_LNE_set_address = 2,
		DW_LNE_define_file = 3,
	};

	std::vector<std::string> m_IncludeDirectories;
	char *m_Binary;

	static ulong uleb128(const char*& p);
	static slong sleb128(const char*& p);
	std::string ParseFilename(const char*& p);
	bool ParseDebugLine(const char*& debug_line, ulong section_size);
	bool ParseSections(int sections, const IMAGE_SECTION_HEADER *SectionHeader);
	bool ParsePeHeader(const IMAGE_NT_HEADERS32 *PeHeader);
	bool ParseDosHeader(const IMAGE_DOS_HEADER *DosHeader);

public:
	CBinaryFile(void) : m_Binary(NULL) { }
	~CBinaryFile() { delete [] m_Binary; }
	bool Open(const char *filename);
	bool Dump(void);
};

ulong CBinaryFile::uleb128(const char*& p) {
	ulong r = 0;
	int s = 0;
	for (;;) {
		ubyte b = *(ubyte*)p++;
		if (b < 0x80) {
			r += b << s;
			break;
		}
		r += (b & 0x7f) << s;
		s += 7;
	}
	return r;
}

slong CBinaryFile::sleb128(const char*& p) {
	slong r = 0;
	int s = 0;
	for (;;) {
		ubyte b = *(ubyte*)p++;
		if (b < 0x80) {
			if (b & 0x40) {
				r -= (0x80 - b) << s;
			}
			else {
				r += (b & 0x3f) << s;
			}
			break;
		}
		r += (b & 0x7f) << s;
		s += 7;
	}
	return r;
}

std::string CBinaryFile::ParseFilename(const char*& p) {
	std::string filename = p;
	while (*p) p++;
	p++;
	ulong dir = uleb128(p);
	std::string dirname;
	if (dir < m_IncludeDirectories.size()) {
		dirname = m_IncludeDirectories[dir];
		if (dirname.length() > 0) {
			dirname += '/';
		}
	}
	// last modified.
	ulong LastModified = uleb128(p);
	// size of the file.
	ulong SizeOfFile = uleb128(p);
	return dirname + filename;
}

bool CBinaryFile::ParseDebugLine(const char*& debug_line, ulong section_size) {
	const char* p = debug_line;

	ulong unit_length = *(uint*)p;
	p += sizeof(uint);
	if (unit_length == 0xffffffff) {
		unit_length = *(ulong*)p;
		p += sizeof(ulong);
	}
	if (unit_length > section_size) {
		printf("Invalid content length\n");
		return false;
	}

	const char* cu_end = p + unit_length;

	int dwarf_version = *(ushort*)p;
	p += sizeof(ushort);
	printf("DWARF version: %d\n", dwarf_version);

	uint header_length = *(uint*)p;
	p += sizeof(uint);

	const char* cu_start = p + header_length;

	uint minimum_instruction_length = *(ubyte*)p;
	p++;

	bool default_is_stmt = *(ubyte*)p != 0;
	p++;

	int line_base = *(sbyte*)p;
	p++;

	uint line_range = *(ubyte*)p;
	p++;

	uint opcode_base = *(ubyte*)p;
	p++;

	std::vector<int> standard_opcode_lengths(opcode_base);
	for (int i = 1; i < (int)opcode_base; i++) {
		standard_opcode_lengths[i] = *(ubyte*)p;
		p++;
	}

	m_IncludeDirectories.push_back("");
	while (*p) {
		m_IncludeDirectories.push_back(p);
		//printf("%s\n", p);
		while (*p) p++;
		p++;
	}
	p++;

	std::vector<std::string> filenames;
	filenames.push_back("");
	while (*p) {
		filenames.push_back(ParseFilename(p));
		//printf("%s\n", filenames.back().c_str());
	}
	p++;

	// The registers.
	addr_t addr = 0;
	uint file = 1;
	uint line = 1;
	uint column = 0;
	bool is_stmt = default_is_stmt;
	bool basic_block = false;
	bool end_sequence = false;
	bool prologue_end = false;
	bool epilogue_begin = false;
	uint isa = 0;

#define DUMP_LINE()												 \
	do {															\
		printf("%s:%d: %p\n", filenames[file].c_str(), line, addr); \
		basic_block = prologue_end = epilogue_begin = false;		\
	} while (0)

	//printf("%d %d %d\n", p-debug_line, unit_length, header_length);
	if (p != cu_start) {
		printf("Unexpected header size\n");
		return false;
	}

	while (p < cu_end) {
		ulong a;
		ubyte op = *p++;
		switch (op) {
		case DW_LNS_copy:
			DUMP_LINE();
			break;
		case DW_LNS_advance_pc:
			a = uleb128(p);
			addr += a;
			break;
		case DW_LNS_advance_line: {
			{
				slong a = sleb128(p);
				line += a;
				//printf("DW_LNS_advance_line %ld => %d\n", a, line);
			}
			break;
		}
		case DW_LNS_set_file:
			file = (uint)uleb128(p);
			break;
		case DW_LNS_set_column:
			column = uleb128(p);
			break;
		case DW_LNS_negate_stmt:
			is_stmt = !is_stmt;
			break;
		case DW_LNS_set_basic_block:
			basic_block = true;
			break;
		case DW_LNS_const_add_pc:
			a = ((255 - opcode_base) / line_range) * minimum_instruction_length;
			addr += a;
			break;
		case DW_LNS_fixed_advance_pc:
			a = *(ubyte*)p++;
			addr += a;
			break;
		case DW_LNS_set_prologue_end:
			prologue_end = true;
			break;
		case DW_LNS_set_epilogue_begin:
			epilogue_begin = true;
			break;
		case DW_LNS_set_isa:
			isa = uleb128(p);
			break;
		case 0:
			a = *(ubyte*)p++;
			op = *p++;
			//printf("extended op: %d size=%d\n", op, a);
			switch (op) {
			case DW_LNE_end_sequence:
				end_sequence = true;
				DUMP_LINE();
				addr = 0;
				file = 1;
				line = 1;
				column = 0;
				is_stmt = default_is_stmt;
				end_sequence = false;
				isa = 0;
				break;
			case DW_LNE_set_address:
				addr = *(addr_t*)p;
				p += sizeof(addr_t);
				break;
			case DW_LNE_define_file:
				filenames.push_back(ParseFilename(p));
				break;
			default:
				printf("Unknown extended opcode: %d\n", op);
				return false;
			}
			break;
		default: {
				a = op - opcode_base;
				uint addr_incr = (a / line_range) * minimum_instruction_length;
				int line_incr = line_base + (a % line_range);
				addr += addr_incr;
				line += line_incr;
				//printf("special: addr +%d => %p, line +%d => %d\n",
				//	   addr_incr, addr, line_incr, line);
				DUMP_LINE();
			}
		}
	}
	debug_line = p;
	return true;
}

bool CBinaryFile::ParseSections(int sections, const IMAGE_SECTION_HEADER *SectionHeader)
{
	int i;
	for (i = 0; i < sections; ++i) {
		char SectionName[IMAGE_SIZEOF_SHORT_NAME + 1];
		memcpy(SectionName, SectionHeader->Name, IMAGE_SIZEOF_SHORT_NAME);
		SectionName[IMAGE_SIZEOF_SHORT_NAME] = '\0';
		if (strcmp(SectionName, "/61") == 0) {
			// /61 == .debug_line
			const char *debug_line = m_Binary + SectionHeader->PointerToRawData;
			ParseDebugLine(debug_line, SectionHeader->SizeOfRawData);
		}
		++SectionHeader;
	}
	return true;
}

bool CBinaryFile::ParsePeHeader(const IMAGE_NT_HEADERS32 *PeHeader)
{
	if (memcmp(&PeHeader->Signature, "PE\0\0", sizeof(PeHeader->Signature)) != 0) {
		printf("No PE header\n");
		return false;
	}
	return ParseSections(PeHeader->FileHeader.NumberOfSections, (const IMAGE_SECTION_HEADER *)
			((const char *)(&PeHeader->OptionalHeader) + PeHeader->FileHeader.SizeOfOptionalHeader));
}

bool CBinaryFile::ParseDosHeader(const IMAGE_DOS_HEADER *DosHeader)
{
	if (memcmp(&DosHeader->e_magic, "MZ", sizeof(DosHeader->e_magic)) != 0) {
		printf("No DOS header\n");
		return false;
	}
	LONG PeOffset = DosHeader->e_lfanew;
	return ParsePeHeader((const IMAGE_NT_HEADERS32 *)((const char *)DosHeader + PeOffset));
}

bool CBinaryFile::Open(const char *filename)
{
	struct stat info;
	FILE *fp;
	if (stat(filename, &info) != 0 || info.st_size <= 0 ||
			(fp = fopen(filename, "rb")) == NULL) {
		printf("Binary file not found\n");
		return false;
	}
	m_Binary = new char[info.st_size];
	fread(m_Binary, 1, info.st_size, fp);
	fclose(fp);

	return true;
}

bool CBinaryFile::Dump()
{
	return ParseDosHeader((const IMAGE_DOS_HEADER *)m_Binary);
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("Usage: %s PE-binary\n", argv[0]);
		return 1;
	}

	CBinaryFile BinaryFile;
	if (!BinaryFile.Open(argv[1])) {
		return 1;
	}

	BinaryFile.Dump();

	return 0;
}

