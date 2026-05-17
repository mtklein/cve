__Z3minDv8_fS_:
	pushq	%rbp
	movq	%rsp, %rbp
	vminps	%ymm0, %ymm1, %ymm2
	vcmpunordps	%ymm0, %ymm0, %ymm0
	vblendvps	%ymm0, %ymm1, %ymm2, %ymm0
	popq	%rbp
	retq
__Z3maxDv8_fS_:
	pushq	%rbp
	movq	%rsp, %rbp
	vmaxps	%ymm0, %ymm1, %ymm2
	vcmpunordps	%ymm0, %ymm0, %ymm0
	vblendvps	%ymm0, %ymm1, %ymm2, %ymm0
	popq	%rbp
	retq
LCPI2_0:
__Z3absDv8_f:
	pushq	%rbp
	movq	%rsp, %rbp
	vbroadcastss	LCPI2_0(%rip), %ymm1
	vandps	%ymm1, %ymm0, %ymm0
	popq	%rbp
	retq
__Z4sqrtDv8_f:
	pushq	%rbp
	movq	%rsp, %rbp
	vsqrtps	%ymm0, %ymm0
	popq	%rbp
	retq
__Z5floorDv8_f:
	pushq	%rbp
	movq	%rsp, %rbp
	vroundps	$9, %ymm0, %ymm0
	popq	%rbp
	retq
__Z4ceilDv8_f:
	pushq	%rbp
	movq	%rsp, %rbp
	vroundps	$10, %ymm0, %ymm0
	popq	%rbp
	retq
LCPI6_0:
LCPI6_1:
__Z5roundDv8_f:
	pushq	%rbp
	movq	%rsp, %rbp
	vbroadcastss	LCPI6_0(%rip), %ymm1
	vandps	%ymm1, %ymm0, %ymm1
	vbroadcastss	LCPI6_1(%rip), %ymm2
	vorps	%ymm2, %ymm1, %ymm1
	vaddps	%ymm1, %ymm0, %ymm0
	vroundps	$11, %ymm0, %ymm0
	popq	%rbp
	retq
__Z3fmaDv8_fS_S_:
	pushq	%rbp
	movq	%rsp, %rbp
	vfmadd213ps	%ymm2, %ymm1, %ymm0
	popq	%rbp
	retq
LCPI8_0:
__Z7reverseDv8_f:
	pushq	%rbp
	movq	%rsp, %rbp
	vmovaps	LCPI8_0(%rip), %ymm1
	vpermps	%ymm0, %ymm1, %ymm0
	popq	%rbp
	retq
__Z6to_i32Dv8_f:
	pushq	%rbp
	movq	%rsp, %rbp
	vcvttps2dq	%ymm0, %ymm0
	popq	%rbp
	retq
