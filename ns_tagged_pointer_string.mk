.PHONY: default

default: ns_tagged_pointer_string
	./$<

ns_tagged_pointer_string: ns_tagged_pointer_string.cc
	g++ -std=c++14 $< -o $@

