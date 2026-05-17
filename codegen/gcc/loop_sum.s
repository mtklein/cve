__Z3sumPKN8cve_impl3vecIfLm4EEEi:
	cmp	w1, 0
	ble	L4
	movi	v31.4s, 0
	sub	w1, w1, #1
	add	x2, x0, 16
	add	x1, x2, w1, uxtw 4
L3:
	ldr	q1, [x0], 16
	fadd	v31.4s, v31.4s, v1.4s
	cmp	x1, x0
	bne	L3
	dup	s29, v31.s[1]
	dup	s30, v31.s[2]
	fadd	s29, s29, s31
	dup	s31, v31.s[3]
	fadd	s30, s29, s30
	fadd	s0, s30, s31
	ret
L4:
	movi	v0.2s, #0
	ret
