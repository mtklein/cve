__Z3sumPKDv4_fi:
	testl	%esi, %esi
	jle	LBB0_1
	movl	%esi, %edx
	movl	%edx, %eax
	andl	$7, %eax
	cmpl	$8, %esi
	jae	LBB0_8
	vxorps	%xmm0, %xmm0, %xmm0
	xorl	%ecx, %ecx
	testq	%rax, %rax
	jne	LBB0_5
	jmp	LBB0_7
LBB0_1:
	vxorps	%xmm0, %xmm0, %xmm0
	jmp	LBB0_7
LBB0_8:
	pushq	%rbp
	movq	%rsp, %rbp
	andl	$2147483640, %edx
	leaq	112(%rdi), %rsi
	vxorps	%xmm0, %xmm0, %xmm0
	xorl	%ecx, %ecx
LBB0_9:
	vaddps	-112(%rsi), %xmm0, %xmm0
	vaddps	-96(%rsi), %xmm0, %xmm0
	vaddps	-80(%rsi), %xmm0, %xmm0
	vaddps	-64(%rsi), %xmm0, %xmm0
	vaddps	-48(%rsi), %xmm0, %xmm0
	vaddps	-32(%rsi), %xmm0, %xmm0
	vaddps	-16(%rsi), %xmm0, %xmm0
	vaddps	(%rsi), %xmm0, %xmm0
	addq	$8, %rcx
	subq	$-128, %rsi
	cmpq	%rcx, %rdx
	jne	LBB0_9
	popq	%rbp
	testq	%rax, %rax
	je	LBB0_7
LBB0_5:
	shlq	$4, %rcx
	addq	%rcx, %rdi
	shll	$4, %eax
	xorl	%ecx, %ecx
LBB0_6:
	vaddps	(%rdi,%rcx), %xmm0, %xmm0
	addq	$16, %rcx
	cmpq	%rcx, %rax
	jne	LBB0_6
LBB0_7:
	vmovshdup	%xmm0, %xmm1
	vaddss	%xmm1, %xmm0, %xmm1
	vshufpd	$1, %xmm0, %xmm0, %xmm2
	vaddss	%xmm1, %xmm2, %xmm1
	vshufps	$255, %xmm0, %xmm0, %xmm0
	vaddss	%xmm1, %xmm0, %xmm0
	retq
