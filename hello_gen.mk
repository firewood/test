
hello: char
	./char > _.c
	$(CC) -c -Wa,-R -w _.c
	$(LD) _.o -e _ -o $@
	./$@

char: chars.cc char.cc
	./hello_gen > chars.cc
	$(CXX) char.cc -o char

hello_gen: hello_gen.cc
	$(CXX) -fno-operator-names -o $@ $<

