__Z3sumPKN8cve_impl3vecIfLm4EEEi:
	movi	v31.4s, 0
	cmp	w1, 0
	ble	L4
	add	x1, x0, w1, uxtw 4
L3:
	ldr	q2, [x0], 16
	fadd	v31.4s, v31.4s, v2.4s
	cmp	x1, x0
	bne	L3
	dup	s1, v31.s[1]
	dup	s30, v31.s[2]
	dup	s0, v31.s[3]
	fadd	s1, s1, s31
	fadd	s30, s1, s30
	fadd	s0, s30, s0
	ret
L4:
	fmov	s0, s31
	ret
