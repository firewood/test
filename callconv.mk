
gcc: callconv.c
	gcc -S $< -o $(<:%.c=%.s).$@

arm64-aapcs: callconv.c
	arm-linux-androideabi-gcc -S $< -o $(<:%.c=%.s).$@

arm64-darwin: callconv.c
	xcrun -sdk iphoneos clang -arch arm64 -S $< -o $(<:%.c=%.s).$@

windows: callconv.c
	cl /c /Fa$(<:%.c=%.s).$@ $<

