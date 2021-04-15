section .text code


; uint64_t fnv1a_64(const char *data, size_t size)
global fnv1a_64_asm
fnv1a_64_asm:
%push fnv1a_64
		
%define		aData rdi
%define		aSize rsi
%define		lHash r8
		
		mov lHash, 0CBF29CE484222325h
		mov r9, 100000001B3h
		xor rax, rax
		add rsi, aData
		
; TODO: Maybe (especially since size is constant) we could operate on bigger chunks of memory?
.loop:		cmp aData, rsi
		jae .end
		
		mov al, [aData]
		xor lHash, rax
		imul lHash, r9
		inc aData
		jmp .loop
		
.end:		mov rax, lHash
		ret

%pop fnv1a_64


; extern "C" uint64_t crc32_asm(const char *data, size_t size);
global crc32_asm
crc32_asm:
%push crc32_asm

		xor rax, rax
		crc32 rax, qword [rdi]
		crc32 rax, qword [rdi+8h]
		crc32 rax, qword [rdi+10h]
		crc32 rax, qword [rdi+18h]
		crc32 rax, qword [rdi+20h]
		crc32 rax, qword [rdi+28h]
		crc32 rax, qword [rdi+30h]
		crc32 rax, qword [rdi+38h]
		ret

%pop crc32_asm

