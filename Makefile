.PHONY: default

default: fingerprint
	./fingerprint key/github.pub

fingerprint: fingerprint.cc
	g++ -g -std=c++14 $< -o $@ -lcrypto -L/usr/local/opt/openssl/lib
