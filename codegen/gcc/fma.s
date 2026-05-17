__Z3fmaN8cve_impl3vecIfLm4EEES1_S1_:
	fmov	d31, x0
	fmov	d30, x2
	fmov	d29, x4
	fmov	v31.d[1], x1
	fmov	v30.d[1], x3
	fmov	v29.d[1], x5
	fmla	v29.4s, v30.4s, v31.4s
	umov	x0, v29.d[0]
	umov	x1, v29.d[1]
	ret
