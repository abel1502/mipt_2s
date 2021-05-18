extern _f@main		; In-program main
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
