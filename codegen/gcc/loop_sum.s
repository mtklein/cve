__Z3sumPKN8cve_impl3vecIfLm4EEEi:
	movi	v1.2d, #0
	cmp	w1, 0
	ble	L4
	add	x1, x0, w1, uxtw 4
L3:
	ldr	q0, [x0], 16
	fadd	v0.4s, v0.4s, v1.4s
	mov	v1.16b, v0.16b
	cmp	x1, x0
	bne	L3
	dup	s30, v0.s[1]
	dup	s31, v0.s[2]
	fadd	s30, s30, s0
	dup	s0, v0.s[3]
	fadd	s31, s30, s31
	fadd	s0, s31, s0
	ret
L4:
	movi	v0.2s, #0
	ret
