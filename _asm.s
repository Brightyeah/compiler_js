.data
.text
.global main
main:
		pushq	%rbp
		movq	%rsp, %rbp
		subq	$16, %rsp
		movl	$3, -4(rbp)
		movl	$2, -8(rbp)
		movq	$.LC0, -16(rbp)
		jmp		.L0:
.L1
		movl	$.LC1, %edi
		movl	$0, %edi
		call	puts
.L0:
		cmpl	$7, -0(%rbp)
		jg		.L1
.L2:
		movl	$9, -20(rbp)
		movl	$0, %eax
		leave
		ret
