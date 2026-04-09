#get_cpuid(int *eaxP, int *ebxP, int *ecxP, int *edxP).
#rdi = eaxP, rsi = ebxP, rdx = edxP, rcx = ecxP
	.text
	.globl get_cpuid

get_cpuid:
	push	%rbx            #must be preserved by callee
	pushq   %rcx            #ecxP; save because changed by cpuid 
	pushq   %rdx            #edxP; save because changed by cpuid
	xorl 	%eax, %eax	#setup cpuid opcode to 0
	cpuid
	#largest param in %eax
	#12-char manufacturer string in ebx, edx, ecx.
	movl	%eax, (%rdi)	#store eax cpuid result
	movl	%ebx, (%rsi)	#store ebx cpuid result
	popq	%rax		#pop address for edxP
	movl    %edx, (%rax)    #store edx cpuid result
	popq    %rax		#pop address for ecxP
	movl	%ecx, (%rax)	#store ecx cpuid result
	pop     %rbx
	ret


#info_cpuid(int op, int *eaxP, int *ebxP, int *ecxP, int *edxP).
#edi = op, rsi = eaxP, rdx = ebxP, rcx = edxP, r8 = ecxP	
	.globl info_cpuid
info_cpuid:
	#TODO
	ret
	
.section .note.GNU-stack,"",@progbits
	
