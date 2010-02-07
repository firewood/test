

//
// dump PE headers
//


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>


//@@
char buffer[1048576];


static bool dump_section_headers( int count, const IMAGE_SECTION_HEADER *p, size_t SizeOfHeaders )
{
//	const BYTE *binary_data = (const BYTE *)(p + count);
//	size_t SectionOffset = (const char *)p - buffer + count * sizeof(IMAGE_SECTION_HEADER);
	size_t SectionOffset = SizeOfHeaders;

	printf( "\nIMAGE_SECTION_HEADER offset: 0x%X\n", (const char *)p - buffer );

	if ( count > 64 ) {
		fprintf( stderr, "too many sections\n" );
		return false;
	}

/*
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
            DWORD   PhysicalAddress;
            DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
*/

	int i;
	for ( i = 0; i < count; ++i ) {
		char SectionName[IMAGE_SIZEOF_SHORT_NAME + 1];
		memcpy( SectionName, p[i].Name, IMAGE_SIZEOF_SHORT_NAME );
		SectionName[IMAGE_SIZEOF_SHORT_NAME] = '\0';

		const char *name = SectionName;
		if ( name[0] != '.' && !isprint( name[0] ) ) {
			name = "unknown";
		}
		if ( strcmp( name, "/4" ) == 0 ) {
			name = ".debug_aranges";
		}
		if ( strcmp( name, "/19" ) == 0 ) {
			name = ".debug_pubnames";
		}
		if ( strcmp( name, "/35" ) == 0 ) {
			name = ".debug_info";
		}
		if ( strcmp( name, "/47" ) == 0 ) {
			name = ".debug_abbrev";
		}
		if ( strcmp( name, "/61" ) == 0 ) {
			name = ".debug_line";
		}
		if ( strcmp( name, "/73" ) == 0 ) {
			name = ".debug_frame";
		}
		if ( strcmp( name, "/86" ) == 0 ) {
			name = ".debug_loc";
		}
		if ( strcmp( name, "/97" ) == 0 ) {
			name = ".debug_ranges";
		}
/*
		if ( p[i].Name[0] != '.' ) {
			fprintf( stderr, "invalid section name\n" );
			return false;
		}
*/

		printf( "  <%s> offset: 0x%x\n", name, SectionOffset );
		printf( "    PhysAddr | VirtSize: 0x%X\n", p[i].Misc.PhysicalAddress );
		printf( "    VirtualAddress: 0x%X\n", p[i].VirtualAddress );
		printf( "    SizeOfRawData: 0x%X\n", p[i].SizeOfRawData );
		printf( "    PointerToRawData: 0x%X\n", p[i].PointerToRawData );
		printf( "    PointerToRelocations: 0x%X\n", p[i].PointerToRelocations );
		printf( "    PointerToLinenumbers: 0x%X\n", p[i].PointerToLinenumbers );
		printf( "    NumberOfRelocations: 0x%X\n", p[i].NumberOfRelocations );
		printf( "    NumberOfLinenumbers: 0x%X\n", p[i].NumberOfLinenumbers );
		printf( "    Characteristics: 0x%X\n", p[i].Characteristics );
		const BYTE *data = (const BYTE *)buffer + p[i].PointerToRawData;
		printf( "    data: %02X %02X %02X %02X...\n", data[0], data[1], data[2], data[3] );

		SectionOffset += p[i].SizeOfRawData;
	}

	return true;
}

static const char *get_magic_string(WORD magic)
{
	switch ( magic ) {
	case IMAGE_NT_OPTIONAL_HDR32_MAGIC:
		return "HDR32";
	case IMAGE_NT_OPTIONAL_HDR64_MAGIC:
		return "HDR64";
	case IMAGE_ROM_OPTIONAL_HDR_MAGIC:
		return "ROM";
	default:
		return "unknown";
	}
}

static const char *get_subsystem_string(WORD subsystem)
{
	switch ( subsystem ) {
	case IMAGE_SUBSYSTEM_NATIVE:
		return "NATIVE";
	case IMAGE_SUBSYSTEM_WINDOWS_GUI:
		return "WINDOWS_GUI";
	case IMAGE_SUBSYSTEM_WINDOWS_CUI:
		return "WINDOWS_CUI";
	case IMAGE_SUBSYSTEM_POSIX_CUI:
		return "POSIX_CUI";
	case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:
		return "WINDOWS_CE_GUI";
	default:
		return "unknown";
	}
}


static const char *data_directory_name[IMAGE_NUMBEROF_DIRECTORY_ENTRIES] = {
		"Export Table", "Import Table", "Resource Table", "Exception Table",
		"Certificate Table", "Base Relocation Table", "Debug", "Architecture",
		"Global Ptr", "TLS Table", "Load Config Table", "Bound Import",
		"IAT", "Delay Import Descriptor", "Reserved", "Reserved"
};

static bool dump_image_data_directories(int count, const IMAGE_DATA_DIRECTORY *p)
{
	printf( "\nIMAGE_DATA_DIRECTORY offset: 0x%X\n", (const char *)p - buffer );

	if ( count > 32 ) {
		fprintf( stderr, "too many image data directories\n" );
		return false;
	}

	int i;
	for ( i = 0; i < count; ++i ) {
		printf( "  <%s> VA: 0x%X, Size: 0x%X\n", data_directory_name[i], p[i].VirtualAddress, p[i].Size );
	}
	return true;
}

static bool dump_optional_header(const IMAGE_OPTIONAL_HEADER32 *p, size_t size)
{
	printf( "\nIMAGE_OPTIONAL_HEADER32 offset: 0x%X\n", (const char *)p - buffer );
	printf( "  Magic: 0x%X (%s)\n", p->Magic, get_magic_string(p->Magic) );
	printf( "  MajorLinkerVersion: 0x%X\n", p->MajorLinkerVersion );
	printf( "  MinorLinkerVersion: 0x%X\n", p->MinorLinkerVersion );
	printf( "  SizeOfCode: 0x%X\n", p->SizeOfCode );
	printf( "  SizeOfInitializedData: 0x%X\n", p->SizeOfInitializedData );
	printf( "  SizeOfUninitializedData: 0x%X\n", p->SizeOfUninitializedData );
	printf( "  AddressOfEntryPoint: 0x%X\n", p->AddressOfEntryPoint );
	printf( "  BaseOfCode: 0x%X\n", p->BaseOfCode );
	printf( "  BaseOfData: 0x%X\n", p->BaseOfData );
	printf( "  ImageBase: 0x%X\n", p->ImageBase );
	printf( "  SectionAlignment: 0x%X\n", p->SectionAlignment );
	printf( "  FileAlignment: 0x%X\n", p->FileAlignment );
	printf( "  MajorOperatingSystemVersion: 0x%X\n", p->MajorOperatingSystemVersion );
	printf( "  MinorOperatingSystemVersion: 0x%X\n", p->MinorOperatingSystemVersion );
	printf( "  MajorImageVersion: 0x%X\n", p->MajorImageVersion );
	printf( "  MinorImageVersion: 0x%X\n", p->MinorImageVersion );
	printf( "  MajorSubsystemVersion: 0x%X\n", p->MajorSubsystemVersion );
	printf( "  MinorSubsystemVersion: 0x%X\n", p->MinorSubsystemVersion );
	printf( "  Win32VersionValue: 0x%X\n", p->Win32VersionValue );
	printf( "  SizeOfImage: 0x%X\n", p->SizeOfImage );
	printf( "  SizeOfHeaders: 0x%X\n", p->SizeOfHeaders );
	printf( "  CheckSum: 0x%X\n", p->CheckSum );
	printf( "  Subsystem: 0x%X (%s)\n", p->Subsystem, get_subsystem_string(p->Subsystem) );
	printf( "  DllCharacteristics: 0x%X\n", p->DllCharacteristics );
	printf( "  SizeOfStackReserve: 0x%X\n", p->SizeOfStackReserve );
	printf( "  SizeOfStackCommit: 0x%X\n", p->SizeOfStackCommit );
	printf( "  SizeOfHeapReserve: 0x%X\n", p->SizeOfHeapReserve );
	printf( "  SizeOfHeapCommit: 0x%X\n", p->SizeOfHeapCommit );
	printf( "  LoaderFlags: 0x%X\n", p->LoaderFlags );
	printf( "  NumberOfRvaAndSizes: 0x%X\n", p->NumberOfRvaAndSizes );

	if ( size > offsetof(IMAGE_OPTIONAL_HEADER32, DataDirectory) ) {
		dump_image_data_directories( (size - offsetof(IMAGE_OPTIONAL_HEADER32, DataDirectory)) / sizeof(IMAGE_DATA_DIRECTORY), p->DataDirectory );
	}
	return true;
}

const char *get_machine_string(WORD machine)
{
	switch ( machine ) {
	case IMAGE_FILE_MACHINE_I386:
		return "x86";
	case IMAGE_FILE_MACHINE_AMD64:
		return "x64";
	default:
		return "unknown";
	}
}

const char *get_timestamp_string(time_t timestamp)
{
	static char buffer[64];
	if ( timestamp >= 0 && timestamp < 0x70000000 ) {
		tm *_tm = localtime( &timestamp );
		sprintf( buffer, "%d/%02d/%02d %02d:%02d:%02d",
				_tm->tm_year + 1900, _tm->tm_mon + 1, _tm->tm_mday,
				_tm->tm_hour, _tm->tm_min, _tm->tm_sec );
	} else {
		strcpy( buffer, "unknown" );
	}
	return buffer;
}

static void or_cat(char *dst, const char *src)
{
	if ( *dst != 0 ) {
		strcat( dst, " | " );
	}
	strcat( dst, src );
}

const char *get_characteristics_string(DWORD characteristics)
{
	static char buffer[256];
	buffer[0] = 0;
	if ( characteristics & IMAGE_FILE_RELOCS_STRIPPED ) {
		or_cat( buffer, "RELOCS_STRIPPED" );
	}
	if ( characteristics & IMAGE_FILE_EXECUTABLE_IMAGE ) {
		or_cat( buffer, "EXECUTABLE_IMAGE" );
	}
	if ( characteristics & IMAGE_FILE_LINE_NUMS_STRIPPED ) {
		or_cat( buffer, "LINE_NUMS_STRIPPED" );
	}
	if ( characteristics & IMAGE_FILE_LOCAL_SYMS_STRIPPED ) {
		or_cat( buffer, "LOCAL_SYMS_STRIPPED" );
	}
	if ( characteristics & IMAGE_FILE_AGGRESIVE_WS_TRIM ) {
		or_cat( buffer, "AGGRESIVE_WS_TRIM" );
	}
	if ( characteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE ) {
		or_cat( buffer, "LARGE_ADDRESS_AWARE" );
	}
	if ( characteristics & IMAGE_FILE_BYTES_REVERSED_LO ) {
		or_cat( buffer, "BYTES_REVERSED_LO" );
	}
	if ( characteristics & IMAGE_FILE_32BIT_MACHINE ) {
		or_cat( buffer, "32BIT_MACHINE" );
	}
	if ( characteristics & IMAGE_FILE_DEBUG_STRIPPED ) {
		or_cat( buffer, "DEBUG_STRIPPED" );
	}
	if ( characteristics & IMAGE_FILE_REMOVABLE_RUN_FROM_SWAP ) {
		or_cat( buffer, "REMOVABLE_RUN_FROM_SWAP" );
	}
	if ( characteristics & IMAGE_FILE_NET_RUN_FROM_SWAP ) {
		or_cat( buffer, "NET_RUN_FROM_SWAP" );
	}
	if ( characteristics & IMAGE_FILE_SYSTEM ) {
		or_cat( buffer, "SYSTEM" );
	}
	if ( characteristics & IMAGE_FILE_DLL ) {
		or_cat( buffer, "DLL" );
	}
	if ( characteristics & IMAGE_FILE_UP_SYSTEM_ONLY ) {
		or_cat( buffer, "UP_SYSTEM_ONLY" );
	}
	if ( characteristics & IMAGE_FILE_BYTES_REVERSED_HI ) {
		or_cat( buffer, "BYTES_REVERSED_HI" );
	}
	return buffer;
}

static bool dump_file_header(const IMAGE_FILE_HEADER *p)
{
	printf( "\nIMAGE_FILE_HEADER offset: 0x%X\n", (const char *)p - buffer );
	printf( "  Machine: 0x%X (%s)\n", p->Machine, get_machine_string(p->Machine) );
	printf( "  NumberOfSections: %d\n", p->NumberOfSections );
	printf( "  TimeDateStamp: 0x%X (%s)\n", p->TimeDateStamp, get_timestamp_string(p->TimeDateStamp) );
	printf( "  PointerToSymbolTable: %d\n", p->PointerToSymbolTable );
	printf( "  NumberOfSymbols: %d\n", p->NumberOfSymbols );
	printf( "  SizeOfOptionalHeader: %d (0x%X)\n", p->SizeOfOptionalHeader, p->SizeOfOptionalHeader );
	printf( "  Characteristics: 0x%X (%s)\n", p->Characteristics, get_characteristics_string(p->Characteristics) );
	return true;
}

static bool dump_image(const IMAGE_NT_HEADERS *p)
{
	if ( p->Signature != 0x00004550 ) {		// 'PE'
		fprintf( stderr, "'PE' signature not found\n" );
		return false;
	}

	return dump_file_header( &p->FileHeader ) &&
			dump_optional_header( &p->OptionalHeader, p->FileHeader.SizeOfOptionalHeader ) &&
			dump_section_headers( p->FileHeader.NumberOfSections, (const IMAGE_SECTION_HEADER *)((const char *)(&p->OptionalHeader) + p->FileHeader.SizeOfOptionalHeader), p->OptionalHeader.SizeOfHeaders );
}

static bool dump(const char *p)
{
	if ( p[0] == 'P' && p[1] == 'E' ) {

	} else if ( p[0] != 'M' || p[1] != 'Z' ) {
		fprintf( stderr, "'MZ' signature not found\n" );
		return false;
	}
	DWORD offset = *(DWORD *)(p + 0x3c);
//	if ( offset < 0x40 || offset > 0x200 ) {
	if ( offset > 0x200 ) {
		fprintf( stderr, "invalid PE position\n" );
		return false;
	}
	printf( "PE offset: 0x%X\n", offset );

	return dump_image( (const IMAGE_NT_HEADERS *)(p + offset) );
}

int main(int argc, char* argv[])
{
	if ( argc == 2 ) {
		FILE *fp = fopen( argv[1], "rb" );
		if ( fp ) {
			size_t count = fread( buffer, 1, sizeof buffer, fp );
			if ( count > 0 && count < sizeof(buffer) ) {
				if ( dump( buffer ) ) {
					return 0;
				}
			}
			fclose( fp );
		}
	}

	fprintf( stderr, "FAILED\n" );
	return 0;
}

