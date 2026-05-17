__Z3sumPKDv4_fi:
	cmp	w1, #1
	b.lt	LBB0_3
	mov	w8, w1
	movi.2d	v0, #0000000000000000
LBB0_2:
	ldr	q1, [x0], #16
	fadd.4s	v0, v0, v1
	subs	x8, x8, #1
	b.ne	LBB0_2
	b	LBB0_4
LBB0_3:
	movi.2d	v0, #0000000000000000
LBB0_4:
	dup.4s	v1, v0[1]
	dup.4s	v2, v0[2]
	fadd.4s	v1, v0, v1
	fadd.4s	v1, v2, v1
	dup.4s	v0, v0[3]
	fadd.4s	v0, v0, v1
	ret
