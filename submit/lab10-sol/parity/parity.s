	.text
	.globl get_parity
#edi contains n	
get_parity:
	testl	%eax, %eax
	setpe	%al
	movzbl	%al, %eax
	ret
.section .note.GNU-stack,"",@progbits
	
