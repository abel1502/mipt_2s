
;format PE console

extern GetStdHandle	; HANDLE WINAPI GetStdHandle(_In_ DWORD nStdHandle);
extern WriteConsoleA	; BOOL WINAPI WriteConsole(_In_ HANDLE  hConsoleOutput, _In_ const VOID *lpBuffer, _In_ DWORD nNumberOfCharsToWrite, _Out_opt_ LPDWORD lpNumberOfCharsWritten, _Reserved_ LPVOID lpReserved);
extern ExitProcess	; void ExitProcess(UINT uExitCode);
extern GetProcessHeap	; HANDLE GetProcessHeap();
extern HeapAlloc	; DECLSPEC_ALLOCATOR LPVOID HeapAlloc(HANDLE hHeap, DWORD  dwFlags, SIZE_T dwBytes);
extern HeapReAlloc	; DECLSPEC_ALLOCATOR LPVOID HeapReAlloc(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem, SIZE_T dwBytes);
extern HeapFree		; BOOL HeapFree(HANDLE hHeap, DWORD dwFlags, _Frees_ptr_opt_ LPVOID lpMem);

extern testCppCallback	; unsigned testCppCallback(unsigned a, unsigned b)


;%include "general.s.inc"


section .text code

global _start
_start:		
		; push 0
		; push 777123o
		; mov r9, 01110111b
		; mov r8, 3802
		; mov rdx, 'Q'
		; mov rcx, Msg
		; sub rsp, byte 32
		; call aprintf
		; add rsp, byte 32
		
		mov ecx, 123
		mov edx, 456
		sub rsp, byte 32
		call testCppCallback
		;add rsp, byte 32
		
		mov rcx, CallbackResultMsg
		mov edx, eax
		;sub rsp, byte 32
		call aprintf
		add rsp, byte 32
		
.end:		xor rcx, rcx
		sub rsp, byte 32
		call ExitProcess
		add rsp, byte 32  ; It's sad I live in a society where this doesn't work
		
		hlt


; =============================================================================
; Standard (windows) x64 ABI; Shadow space REQUIRED
; size_t aprintf(const char *fmt, ...);
global aprintf
aprintf:
%push aprintf

%stacksize	flat64
%assign		%$localsize 0
		; TODO: Check whether I actually need all those to be qwords
%local		bufStart:qword, \
		bufCapacity:qword, \
		heap:qword, \
		curArg:byte, \
		numBufStart:qword

%define		fmt rsi
%define		buf rdi

%assign		DEFAULT_BUF_SIZE 100h
%assign		NUMBUF_SIZE 20h

%define		arg(i)  [rbp + ((i)+2)*8h]

		enter %$localsize, 0
		push rsi
		push rdi
		push r12
		push r13
		push r14
		
		;mov arg(0), rcx
		mov arg(1), rdx
		mov arg(2), r8
		mov arg(3), r9
		
		mov fmt, rcx
		mov [curArg], byte 1  ; Actually the shortest option
		xor rax, rax
		mov [bufStart], rax
		mov [numBufStart], rax
		
		mov eax, DEFAULT_BUF_SIZE  ; I believe this should zero out the top bytes
		mov [bufCapacity], rax
		
		sub rsp, byte 32
		call GetProcessHeap
		add rsp, byte 32
		mov [heap], rax
		
		mov rcx, rax	; default heap
		xor rdx, rdx
		mov dl, 8	; HEAP_ZERO_MEMORY
		mov r8, [bufCapacity]
		sub rsp, byte 32
		call HeapAlloc
		add rsp, byte 32
		mov [bufStart], rax
		mov buf, rax
		
		test rax, rax
		jz .end
		
		mov rcx, [heap]	; default heap
		xor rdx, rdx
		mov dl, 8	; HEAP_ZERO_MEMORY
		mov r8d, NUMBUF_SIZE
		sub rsp, byte 32
		call HeapAlloc
		add rsp, byte 32
		mov [numBufStart], rax
		
		test rax, rax
		jz .end
		
.loop:		xor rax, rax
		mov al, [fmt]
		test al, al
		jz .end
		
		cmp al, '%'
		je .percent
		jmp .regularChar
		
.percent:	; '%', 'b', 'c', 'd', 'o', 's', 'u', 'x'
		; TODO: Macros?
		
		inc fmt
		
		mov al, [fmt]
		
		sub al, '%'
		jb .percentUnreg
		
		cmp al, 'x' - '%'
		ja .percentUnreg
		
		lea rcx, [rel .jumpTable1]
		mov al, byte [rcx + rax]
		add rcx, .jumpTable2 - .jumpTable1
		mov eax, dword [rcx + rax * 4]
		sub rcx, .jumpTable2 wrt ..imagebase
		add rax, rcx
		jmp rax
		
		jmp .loop

.percentEsc:	xor rcx, rcx
		mov cl, '%'
		call .putc
		test rax, rax
		jnz .end
		inc fmt
		inc buf
		jmp .loop

.percentB:	xor rax, rax
		mov al, byte [curArg]
		mov rcx, arg(rax)
		inc al
		mov byte [curArg], al
		call .putNumBin
		test rax, rax
		jnz .end
		inc fmt
		jmp .loop
		
.percentC:	xor rax, rax
		mov al, byte [curArg]
		mov rcx, arg(rax)
		inc al
		mov byte [curArg], al
		call .putc
		inc fmt
		jmp .loop
		
.percentD:	xor rax, rax
		mov al, byte [curArg]
		mov rcx, arg(rax)
		inc al
		mov byte [curArg], al
		xor rdx, rdx
		inc rdx
		call .putNumD
		test rax, rax
		jnz .end
		inc fmt
		jmp .loop
		
.percentO:	xor rax, rax
		mov al, byte [curArg]
		mov rcx, arg(rax)
		inc al
		mov byte [curArg], al
		call .putNumOct
		test rax, rax
		jnz .end
		inc fmt
		jmp .loop
		
.percentS:	xor rax, rax
		mov al, byte [curArg]
		mov rdx, arg(rax)
		inc al
		mov byte [curArg], al
		
		;int3
		
		xor rcx, rcx
		dec rcx
		mov r12, rdi
		xor al, al
		mov rdi, rdx
		repne scasb
		mov rdi, r12
		
		inc rcx
		neg rcx
		
		call .puts
		test rax, rax
		jnz .end
		inc fmt
		
		jmp .loop
		
.percentU:	xor rax, rax
		mov al, byte [curArg]
		mov rcx, arg(rax)
		inc al
		mov byte [curArg], al
		xor rdx, rdx
		call .putNumD
		test rax, rax
		jnz .end
		inc fmt
		jmp .loop
		
.percentX:	xor rax, rax
		mov al, byte [curArg]
		mov rcx, arg(rax)
		inc al
		mov byte [curArg], al
		call .putNumHex
		test rax, rax
		jnz .end
		inc fmt
		jmp .loop
		
.percentUnreg:	xor rcx, rcx
		mov cl, '%'
		call .putc
		test rax, rax
		jnz .end
		
		; Fallthrough
		
.regularChar:	xor rcx, rcx
		mov cl, [fmt]
		call .putc
		test rax, rax
		jnz .end
		inc fmt
		jmp .loop

; TODO: jump table
.jumpTable1:
		db 1 ; %
times 'b'-'%'-1 db 0
		db 2 ; b
times 'c'-'b'-1 db 0
		db 3 ; c
times 'd'-'c'-1 db 0
		db 4 ; d
times 'o'-'d'-1 db 0
		db 5 ; o
times 's'-'o'-1 db 0
		db 6 ; s
times 'u'-'s'-1 db 0
		db 7 ; u
times 'x'-'u'-1 db 0
		db 8 ; x

.jumpTable2:
		dd .percentUnreg wrt ..imagebase
		dd .percentEsc wrt ..imagebase
		dd .percentB wrt ..imagebase
		dd .percentC wrt ..imagebase
		dd .percentD wrt ..imagebase
		dd .percentO wrt ..imagebase
		dd .percentS wrt ..imagebase
		dd .percentU wrt ..imagebase
		dd .percentX wrt ..imagebase
		
.end:		mov rax, buf
		sub rax, [bufStart]
		
		mov r12, rax
		
		mov rcx, -11
		sub rsp, byte 32
		call GetStdHandle
		add rsp, byte 32
		test rax, rax
		jz .endNoprint
		
		xor r9, r9
		push r9
		mov r8, r12
		mov rdx, [bufStart]
		mov rcx, rax
		sub rsp, byte 32
		call WriteConsoleA  ; We output whatever we have managed to assemble - even if we're exiting due to an error
		add rsp, byte 32
		
.endNoprint:	mov rcx, [heap]	; default heap
		xor rdx, rdx	; no flags
		mov r8, [bufStart] ; is allowed to be NULL
		call HeapFree	; Technically, this return success status, but there's no point in checking it now...
		
		mov rcx, [heap]	; same
		xor rdx, rdx
		mov r8, [numBufStart]
		call HeapFree
		
		mov rax, r12
		
		pop r14
		pop r13 ; TODO: Some may be unused, should consider moving hot locals here
		pop r12
		pop rdi
		pop rsi
		leave
		
		ret


; Subfunc (so the calling convention may not be strictly obeyed here, I assume); non-zero result indicates error
; bool putc(char c);
.putc:
		mov rdx, [bufStart]
		add rdx, [bufCapacity]
		cmp rdx, buf
		
		jnz .putc.skipResize
		
		mov r12, rcx
		call .resize
		mov rcx, r12
		
		test rax, rax
		jz .putc.skipResize
		
		ret
		
.putc.skipResize:
		mov [buf], cl
		inc buf
		
		xor rax, rax
		
		ret


; Subfunc; non-zero result indicates error; 
; EXACTLY length bytes are written, not just up to length;
; bool puts(size_t length, const char *s);
.puts:		; TODO
		
		mov r8, [bufStart]
		add r8, [bufCapacity]
		sub r8, buf
		sub r8, rcx
		
		jg .puts.skipResize
		
		mov r12, rcx
		mov r13, rdx
		call .resize
		mov rcx, r12
		mov rdx, r13
		
		test rax, rax
		jz .puts  ; Repeat to see if another resize is needed
		
		ret
		
.puts.skipResize:
		
		mov r12, rsi
		mov rsi, rdx
		repnz movsb
		mov rsi, r12
		
		xor rax, rax
		
		ret


; Subfunc. Scales the buffer up two times; non-zero result indicates error
; bool resize();
.resize:	
		mov rcx, [heap]
		xor rdx, rdx
		mov dl, 8
		mov r8, [bufStart]
		sub buf, r8
		mov r9, [bufCapacity]
		shl r9, 1
		mov [bufCapacity], r9
		
		sub rsp, byte 32
		call HeapReAlloc
		add rsp, byte 32
		
		test rax, rax
		; This is an error, and we have no real way to deal with it
		
		; jz .end  ; - Can't do that, we're technically inside another function 
		;              and there's an extra return address on the stack.
		
		; But this oughtta do
		jnz .resize.skip
		
		xor rax, rax
		inc rax
		
		ret
		
.resize.skip:	
		mov [bufStart], rax
		add buf, rax
		
		xor rax, rax
		
		ret


%macro putNumLg 2
; Subfunc; non-zero result indicates error
; bool %2(unsigned num);
.%2:		
		mov ecx, ecx  ; TODO: Maybe movsx
		
		mov r12, rdi
		mov rdi, [numBufStart]
		
.%2.loop:	
		test rcx, rcx
		jz .%2.endLoop
		
		
		mov rax, rcx
		and rax, (1 << %1) - 1
		;bzhi rax rcx %1
		shr rcx, %1
		
		cmp al, 10
		jae .%2.bigDigit
		
		add al, '0'
		
		jmp .%2.notBigDigit
.%2.bigDigit:	
		add al, 'A' - 10
.%2.notBigDigit:

		mov [rdi], al
		inc rdi
		jmp .%2.loop
.%2.endLoop:	
		mov r8, rdi
		sub r8, [numBufStart]
		
		test r8, r8
		jnz .%2.nonZero
		
		mov [rdi], byte '0'
		inc rdi
		inc r8
		
.%2.nonZero:	
		xor rcx, rcx
		dec rdi
		
.%2.loopReverse:
		mov rax, [numBufStart]
		mov dl, [rax + rcx]
		xchg dl, [rdi]
		mov [rax + rcx], dl
		
		inc rcx
		dec rdi
		
		mov rax, [numBufStart]
		lea rax, [rax + rcx]
		cmp rax, rdi
		jae .%2.endLoopReverse
		
		jmp .%2.loopReverse
.%2.endLoopReverse:

		mov rdi, r12
		
		mov rcx, r8
		mov rdx, [numBufStart]
		call .puts
		
		ret
%endmacro

putNumLg 1, putNumBin
putNumLg 3, putNumOct
putNumLg 4, putNumHex

; Subfunc; non-zero result indicates error
; bool putNumD(unsigned num, bool considerSign);
.putNumD:	
		movsx rcx, ecx
		
		mov r12, rdi
		mov rdi, [numBufStart]
		
		test rcx, rcx
		jns .putNumD.noSign
		test rdx, rdx
		jz .putNumD.noSign
		
		neg rcx
		mov [rdi], byte '-'
		inc rdi
		
.putNumD.noSign:
		
.putNumD.loop:	
		test rcx, rcx
		jz .putNumD.endLoop
		
		xor rdx, rdx
		mov rax, rcx
		mov ecx, 10
		div rcx
		mov rcx, rax
		
		mov eax, edx
		
		add al, '0'
		mov [rdi], al
		inc rdi
		jmp .putNumD.loop
.putNumD.endLoop:
		
		mov r8, rdi
		sub r8, [numBufStart]
		
		test r8, r8
		jnz .putNumD.nonZero
		
		mov [rdi], byte '0'
		inc rdi
		inc r8
		
.putNumD.nonZero:	
		xor rcx, rcx
		dec rdi
		
		mov rax, [numBufStart]
		mov al, [rax]
		cmp al, '-'
		jne .putNumD.loopReverse
		inc rcx
		
.putNumD.loopReverse:
		mov rax, [numBufStart]
		mov dl, [rax + rcx]
		xchg dl, [rdi]
		mov [rax + rcx], dl
		
		inc rcx
		dec rdi
		
		mov rax, [numBufStart]
		lea rax, [rax + rcx]
		cmp rax, rdi
		jae .putNumD.endLoopReverse
		
		jmp .putNumD.loopReverse
.putNumD.endLoopReverse:

		mov rdi, r12
		
		mov rcx, r8
		mov rdx, [numBufStart]
		call .puts
		
		ret
		
		ret

		
%pop aprintf
; =============================================================================


section .data data
		
Msg		db `Hello :)\n'%c', 0x%x, 0b%b, 0o%o.\nZero is printed like %x.\n\0`
MsgLen		equ $ - Msg - 1

CallbackResultMsg \
		db `// ... and got 0x%x\n\0`
CallbackResultMsgLen \
		equ $ - CallbackResultMsg - 1