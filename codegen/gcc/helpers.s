__Z3minN8cve_impl3vecIfLm8EEES1_:
	ldp	q31, q30, [x0]
	ldp	q29, q28, [x1]
	fcmgt	v27.4s, v28.4s, v30.4s
	fcmgt	v26.4s, v29.4s, v31.4s
	bsl	v27.16b, v30.16b, v28.16b
	bsl	v26.16b, v31.16b, v29.16b
	stp	q26, q27, [x8]
	ret
__Z3maxN8cve_impl3vecIfLm8EEES1_:
	ldp	q31, q30, [x0]
	ldp	q29, q28, [x1]
	fcmgt	v27.4s, v30.4s, v28.4s
	fcmgt	v26.4s, v31.4s, v29.4s
	bsl	v27.16b, v30.16b, v28.16b
	bsl	v26.16b, v31.16b, v29.16b
	stp	q26, q27, [x8]
	ret
__Z3absN8cve_impl3vecIfLm8EEE:
	ldp	s31, s30, [x0]
	ldp	s29, s28, [x0, 8]
	ldp	s27, s26, [x0, 16]
	fcmpe	s31, #0.0
	ldp	s25, s24, [x0, 24]
	bmi	L21
L5:
	fcmpe	s30, #0.0
	bmi	L22
L7:
	fcmpe	s29, #0.0
	bmi	L23
L9:
	fcmpe	s28, #0.0
	bmi	L24
L11:
	fcmpe	s27, #0.0
	bmi	L25
L13:
	fcmpe	s26, #0.0
	bmi	L26
L15:
	fcmpe	s25, #0.0
	bmi	L27
L17:
	fcmpe	s24, #0.0
	bmi	L28
	stp	s31, s30, [x8]
	stp	s29, s28, [x8, 8]
	stp	s27, s26, [x8, 16]
	stp	s25, s24, [x8, 24]
	ret
L21:
	fneg	s31, s31
	b	L5
L28:
	fneg	s24, s24
	stp	s31, s30, [x8]
	stp	s29, s28, [x8, 8]
	stp	s27, s26, [x8, 16]
	stp	s25, s24, [x8, 24]
	ret
L27:
	fneg	s25, s25
	b	L17
L26:
	fneg	s26, s26
	b	L15
L25:
	fneg	s27, s27
	b	L13
L24:
	fneg	s28, s28
	b	L11
L23:
	fneg	s29, s29
	b	L9
L22:
	fneg	s30, s30
	b	L7
__Z4sqrtN8cve_impl3vecIfLm8EEE:
	stp	x29, x30, [sp, -64]!
	mov	x29, sp
	ldp	s25, s24, [x0]
	ldp	s27, s26, [x0, 8]
	ldp	s29, s28, [x0, 16]
	fcmp	s25, #0.0
	ldp	s31, s30, [x0, 24]
	bpl	L54
	fmov	s0, s25
	str	x8, [x29, 24]
	stp	s24, s27, [x29, 36]
	stp	s26, s29, [x29, 44]
	stp	s28, s31, [x29, 52]
	str	s30, [x29, 60]
	bl	_sqrtf
	ldr	x8, [x29, 24]
	fmov	s25, s0
	ldr	s30, [x29, 60]
	ldp	s24, s27, [x29, 36]
	ldp	s26, s29, [x29, 44]
	ldp	s28, s31, [x29, 52]
	b	L32
L54:
	fsqrt	s25, s25
L32:
	fcmp	s24, #0.0
	bpl	L55
	fmov	s0, s24
	str	x8, [x29, 24]
	stp	s27, s26, [x29, 36]
	stp	s29, s28, [x29, 44]
	stp	s25, s31, [x29, 52]
	str	s30, [x29, 60]
	bl	_sqrtf
	ldr	x8, [x29, 24]
	fmov	s24, s0
	ldr	s30, [x29, 60]
	ldp	s27, s26, [x29, 36]
	ldp	s29, s28, [x29, 44]
	ldp	s25, s31, [x29, 52]
	b	L35
L55:
	fsqrt	s24, s24
L35:
	fcmp	s27, #0.0
	bpl	L56
	fmov	s0, s27
	str	x8, [x29, 24]
	stp	s26, s29, [x29, 36]
	stp	s28, s24, [x29, 44]
	stp	s25, s31, [x29, 52]
	str	s30, [x29, 60]
	bl	_sqrtf
	ldr	x8, [x29, 24]
	fmov	s27, s0
	ldr	s30, [x29, 60]
	ldp	s26, s29, [x29, 36]
	ldp	s28, s24, [x29, 44]
	ldp	s25, s31, [x29, 52]
	b	L38
L56:
	fsqrt	s27, s27
L38:
	fcmp	s26, #0.0
	bpl	L57
	fmov	s0, s26
	str	x8, [x29, 24]
	stp	s29, s28, [x29, 36]
	stp	s27, s24, [x29, 44]
	stp	s25, s31, [x29, 52]
	str	s30, [x29, 60]
	bl	_sqrtf
	ldr	x8, [x29, 24]
	fmov	s26, s0
	ldr	s30, [x29, 60]
	ldp	s29, s28, [x29, 36]
	ldp	s27, s24, [x29, 44]
	ldp	s25, s31, [x29, 52]
	b	L41
L57:
	fsqrt	s26, s26
L41:
	fcmp	s29, #0.0
	bpl	L58
	fmov	s0, s29
	str	x8, [x29, 24]
	stp	s28, s26, [x29, 36]
	stp	s27, s24, [x29, 44]
	stp	s25, s31, [x29, 52]
	str	s30, [x29, 60]
	bl	_sqrtf
	ldr	x8, [x29, 24]
	fmov	s29, s0
	ldr	s30, [x29, 60]
	ldp	s28, s26, [x29, 36]
	ldp	s27, s24, [x29, 44]
	ldp	s25, s31, [x29, 52]
	b	L44
L58:
	fsqrt	s29, s29
L44:
	fcmp	s28, #0.0
	bpl	L59
	fmov	s0, s28
	str	x8, [x29, 24]
	stp	s29, s26, [x29, 36]
	stp	s27, s24, [x29, 44]
	stp	s25, s31, [x29, 52]
	str	s30, [x29, 60]
	bl	_sqrtf
	ldr	x8, [x29, 24]
	fmov	s28, s0
	ldr	s30, [x29, 60]
	ldp	s29, s26, [x29, 36]
	ldp	s27, s24, [x29, 44]
	ldp	s25, s31, [x29, 52]
	b	L47
L59:
	fsqrt	s28, s28
L47:
	fcmp	s31, #0.0
	bpl	L60
	fmov	s0, s31
	str	x8, [x29, 24]
	stp	s28, s29, [x29, 36]
	stp	s26, s27, [x29, 44]
	stp	s24, s25, [x29, 52]
	str	s30, [x29, 60]
	bl	_sqrtf
	ldr	x8, [x29, 24]
	fmov	s31, s0
	ldr	s30, [x29, 60]
	ldp	s28, s29, [x29, 36]
	ldp	s26, s27, [x29, 44]
	ldp	s24, s25, [x29, 52]
	b	L50
L60:
	fsqrt	s31, s31
L50:
	fcmp	s30, #0.0
	bpl	L61
	fmov	s0, s30
	str	x8, [x29, 24]
	stp	s31, s28, [x29, 36]
	stp	s29, s26, [x29, 44]
	stp	s27, s24, [x29, 52]
	str	s25, [x29, 60]
	bl	_sqrtf
	ldr	x8, [x29, 24]
	fmov	s30, s0
	ldr	s25, [x29, 60]
	ldp	s31, s28, [x29, 36]
	ldp	s29, s26, [x29, 44]
	ldp	s27, s24, [x29, 52]
	b	L53
L61:
	fsqrt	s30, s30
L53:
	stp	s25, s24, [x8]
	stp	s27, s26, [x8, 8]
	stp	s29, s28, [x8, 16]
	stp	s31, s30, [x8, 24]
	ldp	x29, x30, [sp], 64
	ret
__Z5floorN8cve_impl3vecIfLm8EEE:
	ldp	q30, q31, [x0]
	frintm	v30.4s, v30.4s
	frintm	v31.4s, v31.4s
	stp	q30, q31, [x8]
	ret
__Z4ceilN8cve_impl3vecIfLm8EEE:
	ldp	q30, q31, [x0]
	frintp	v30.4s, v30.4s
	frintp	v31.4s, v31.4s
	stp	q30, q31, [x8]
	ret
__Z5roundN8cve_impl3vecIfLm8EEE:
	ldp	q30, q31, [x0]
	frinta	v30.4s, v30.4s
	frinta	v31.4s, v31.4s
	stp	q30, q31, [x8]
	ret
__Z3fmaN8cve_impl3vecIfLm8EEES1_S1_:
	ldp	q28, q30, [x1]
	ldp	q27, q31, [x0]
	ldp	q26, q29, [x2]
	fmla	v26.4s, v28.4s, v27.4s
	fmla	v29.4s, v30.4s, v31.4s
	stp	q26, q29, [x8]
	ret
__Z7reverseN8cve_impl3vecIfLm8EEE:
	ldp	q31, q30, [x0]
	adrp	x0, lC0@PAGE
	ldr	q29, [x0, #lC0@PAGEOFF]
	tbl	v30.16b, {v30.16b}, v29.16b
	tbl	v29.16b, {v31.16b}, v29.16b
	stp	q30, q29, [x8]
	ret
__Z6to_i32N8cve_impl3vecIfLm8EEE:
	ldp	q30, q31, [x0]
	fcvtzs	v30.4s, v30.4s
	fcvtzs	v31.4s, v31.4s
	stp	q30, q31, [x8]
	ret
	.literal16
lC0:
