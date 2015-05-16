
default: hello
	./$<

hello: _.c
	$(CC) -c -Wa,-R -w $<
	$(LD) _.o -e _ -o $@

_.c: hello_gen
	./$< > $@

hello_gen: hello_gen.cc numbers.cc
	$(CXX) -o $@ $<

numbers.cc: hello_gen_gen
	./$< > $@

hello_gen_gen: hello_gen_gen.cc
	$(CXX) -fno-operator-names -o $@ $<

