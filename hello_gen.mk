
hello: hello_gen
	./$< > _.c
	$(CC) -c -Wa,-R -w _.c
	$(LD) _.o -e _ -o $@
	-./$@

hello_gen: hello_gen.cc
	$(CXX) -fno-operator-names -o $@ $<

