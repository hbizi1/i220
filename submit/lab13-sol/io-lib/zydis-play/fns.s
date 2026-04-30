	.text
	.globl disasm_fn

disasm_fn:

	mov  	%rax, 22(%rax)
	mov 	1234(%rbx, %rcx), %rsi
	movl    $0xdeadbeef, 22(%rbx)
	movb	$0x22, 33(%rax, %rcx)
	addb	%al, (%rdi)
	movw	%bx, (%rcx)

	.globl end_fn
end_fn:	

.section .note.GNU-stack,"",@progbits
	
