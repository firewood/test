BITS 32
ORG  0

WriteFile    equ 0x7c810d87 ; Varies depending on environments
ImageBase    equ 0x00400000

filealign equ 4
sectalign equ 4   ; must be 4 because of e_lfanew

%define round(n, r) (((n+(r-1))/r)*r)

	dw "MZ"
	dw 0
pe_hdr	dd "PE"		; Signature
	dw 0x014C		; CPU: i386
	dw 1		; Number of Sections
hello	db "Hello,world"
hello_e	db 0
	dw 4			; offset between sections and opthdr
	dw 0x0103		; Characteristics: RELOCS_STRIPPED|EXECUTABLE|32BI
opthdr:
	dw 0x010B		; Magic
code1:
	mov edx, hello + ImageBase
	push eax
	push ebx
	jmp short code2
	db 0		; not used

	dd codesize
	dd code1		; 
	dd codesize
	dd code1		; EntryPoint
	dd ImageBase	; ImageBase Address
sectbl:	dd 4		; PE hdr / Section Alignment
	dd 4		; File Alignment
code2:
	push byte hello_e - hello
	push edx
	push byte 7	; stdout handle
	jmp short code3
	db 0		; not used

	dw 4		; Subsys Major Ver
code3:
	call WriteFile - ImageBase
	ret

	dd round(hdrsize, sectalign)+round(codesize,sectalign) ; SizeOfImage
	dd round(hdrsize, filealign)              ; SizeOfHeaders
	dd 0
	db 3		; Subsystem:console

codesize equ $ - code1

hdrsize equ $ - $$

filesize equ $ - $$

