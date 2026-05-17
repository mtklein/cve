__Z3minDv8_fS_:
	ldp	q1, q0, [x0]
	ldp	q3, q2, [x1]
	fminnm.4s	v1, v1, v3
	fminnm.4s	v0, v0, v2
	stp	q1, q0, [x8]
	ret
__Z3maxDv8_fS_:
	ldp	q1, q0, [x0]
	ldp	q3, q2, [x1]
	fmaxnm.4s	v1, v1, v3
	fmaxnm.4s	v0, v0, v2
	stp	q1, q0, [x8]
	ret
__Z3absDv8_f:
	ldp	q1, q0, [x0]
	fabs.4s	v1, v1
	fabs.4s	v0, v0
	stp	q1, q0, [x8]
	ret
__Z4sqrtDv8_f:
	ldp	q1, q0, [x0]
	fsqrt.4s	v1, v1
	fsqrt.4s	v0, v0
	stp	q1, q0, [x8]
	ret
__Z5floorDv8_f:
	ldp	q1, q0, [x0]
	frintm.4s	v1, v1
	frintm.4s	v0, v0
	stp	q1, q0, [x8]
	ret
__Z4ceilDv8_f:
	ldp	q1, q0, [x0]
	frintp.4s	v1, v1
	frintp.4s	v0, v0
	stp	q1, q0, [x8]
	ret
__Z5roundDv8_f:
	ldp	q1, q0, [x0]
	frinta.4s	v1, v1
	frinta.4s	v0, v0
	stp	q1, q0, [x8]
	ret
__Z3fmaDv8_fS_S_:
	ldp	q1, q0, [x0]
	ldp	q3, q2, [x1]
	ldp	q5, q4, [x2]
	fmla.4s	v5, v3, v1
	fmla.4s	v4, v2, v0
	stp	q5, q4, [x8]
	ret
__Z7reverseDv8_f:
	ldp	q0, q1, [x0]
	rev64.4s	v1, v1
	ext.16b	v1, v1, v1, #8
	rev64.4s	v0, v0
	ext.16b	v0, v0, v0, #8
	stp	q1, q0, [x8]
	ret
__Z6to_i32Dv8_f:
	ldp	q1, q0, [x0]
	fcvtzs.4s	v1, v1
	fcvtzs.4s	v0, v0
	stp	q1, q0, [x8]
	ret
