
;format PE console

extern _f@main		; In-program main

extern printf
extern scanf

extern GetStdHandle	; HANDLE WINAPI GetStdHandle(_In_ DWORD nStdHandle);
extern WriteConsoleA	; BOOL WINAPI WriteConsole(_In_ HANDLE  hConsoleOutput, _In_ const VOID *lpBuffer, _In_ DWORD nNumberOfCharsToWrite, _Out_opt_ LPDWORD lpNumberOfCharsWritten, _Reserved_ LPVOID lpReserved);
extern ExitProcess	; void ExitProcess(UINT uExitCode);



section .text code

global _startNoCRT
_startNoCRT:	
		call _f@main
		
		
.end:		xor rcx, rcx
		sub rsp, byte 20h
		call ExitProcess
		add rsp, byte 20h  ; It's sad I live in a society where this doesn't work
		
		hlt


; =============================================================================

global main
main:
		push rbp
		push rsi
		push rdi
		push r12
		push r13
		push r14
		push r15
		push rbx
		
		add rsp, byte 8
		call _f@main
		sub rsp, byte 8
		
		pop rbx
		pop r15
		pop r14
		pop r13
		pop r12
		pop rdi
		pop rsi
		pop rbp
		
		xor rax, rax
		ret


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
