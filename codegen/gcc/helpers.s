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
	ldp	q31, q30, [x0]
	fabs	v31.4s, v31.4s
	fabs	v30.4s, v30.4s
	stp	q31, q30, [x8]
	ret
__Z4sqrtN8cve_impl3vecIfLm8EEE:
	ldp	q30, q31, [x0]
	fsqrt	v30.4s, v30.4s
	fsqrt	v31.4s, v31.4s
	stp	q30, q31, [x8]
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
