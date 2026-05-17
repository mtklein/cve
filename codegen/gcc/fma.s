__Z3fmaN8cve_impl3vecIfLm4EEES1_S1_:
	fmov	w0, s0
	mov	x3, 0
	fmov	w4, s4
	mov	x1, 0
	mov	x2, 0
	sub	sp, sp, #32
	ldr	q31, [sp, 32]
	bfi	x3, x0, 0, 32
	fmov	w0, s1
	bfi	x1, x4, 0, 32
	fmov	w4, s5
	bfi	x3, x0, 32, 32
	fmov	w0, s2
	bfi	x1, x4, 32, 32
	fmov	w4, s6
	bfi	x2, x0, 0, 32
	fmov	w0, s3
	bfi	x2, x0, 32, 32
	mov	x0, 0
	bfi	x0, x4, 0, 32
	fmov	w4, s7
	stp	x3, x2, [sp]
	bfi	x0, x4, 32, 32
	stp	x1, x0, [sp, 16]
	ldp	q29, q30, [sp]
	add	sp, sp, 32
	fmla	v31.4s, v30.4s, v29.4s
	umov	x0, v31.d[0]
	fmov	s0, s31
	umov	x1, v31.d[1]
	lsr	x3, x0, 32
	lsr	x0, x1, 32
	fmov	s2, w1
	fmov	s1, w3
	fmov	s3, w0
	ret
