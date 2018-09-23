
gcc: callconv.c
	gcc -S $< -o $<.$@

arm64-aapcs: callconv.c
	arm-linux-androideabi-gcc -S $< -o $<.$@

arm64-darwin: callconv.c
	xcrun -sdk iphoneos clang -arch arm64 -S $< -o $<.$@

