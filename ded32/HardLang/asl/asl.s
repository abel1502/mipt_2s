extern printf
extern scanf

section .text code

; def writeInt4(int4:val);
global _f@writeInt4
_f@writeInt4:	
		mov edx, [rsp + 8h]
		mov rcx, FmtInt4n
		jmp printf

; def writeInt8(int8:val);
global _f@writeInt8
_f@writeInt8:	
		mov rdx, [rsp + 8h]
		mov rcx, FmtInt8n
		jmp printf
		
; def writeDbl(dbl:val);
global _f@writeDbl
_f@writeDbl:	
		movq xmm1, [rsp + 8h]
		mov rdx, [rsp + 8h]
		mov rcx, FmtDbln
		jmp printf
		
; def int4:readInt4();
global _f@readInt4
_f@readInt4:	
		push rbp
		mov rbp, rsp
		lea rdx, [rbp - 8h]
		mov rcx, FmtInt4
		sub rsp, byte 30h
		call scanf
		mov r10d, [rbp - 8]
		mov rsp, rbp
		pop rbp
		
		ret

; def int8:readInt8();
global _f@readInt8
_f@readInt8:	
		push rbp
		mov rbp, rsp
		lea rdx, [rbp - 8]
		mov rcx, FmtInt8
		sub rsp, byte 30h
		call scanf
		mov r10, [rbp - 8]
		mov rsp, rbp
		pop rbp
		
		ret
		
; def dbl:readDbl();
global _f@readDbl
_f@readDbl:	
		push rbp
		mov rbp, rsp
		lea rdx, [rbp - 8]
		mov rcx, FmtDbl
		sub rsp, byte 30h
		call scanf
		mov r10, [rbp - 8]
		mov rsp, rbp
		pop rbp
		
		ret

		

; =============================================================================


section .data data
		
FmtInt4		db `%d\0`
FmtInt4n	db `%d\n\0`
FmtInt8		db `%lld\0`
FmtInt8n	db `%lld\n\0`
FmtDbl		db `%lg\0`
FmtDbln		db `%lg\n\0`
BufScanf	dd 0
