
#hello: char
#	./char > _.c
#	$(CC) -c -Wa,-R -w _.c
#	$(LD) _.o -e _ -o $@
#	./$@

#char: chars.cc char.cc
#	./hello_gen > chars.cc
#	$(CXX) char.cc -o char


default: hello
	./$<

hello: _.c
	$(CC) -c -Wa,-R -w $<
	$(LD) _.o -e _ -o $@

_.c: hello_gen
	./$< > $@

hello_gen: hello_gen.cc chars.cc
	$(CXX) -o $@ $<

chars.cc: hello_gen_gen
	./$< > $@

hello_gen_gen: hello_gen_gen.cc
	$(CXX) -fno-operator-names -o $@ $<

