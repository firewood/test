#include <iostream>
#include <string>

using namespace std;

template <unsigned int n> struct Char;

template <> struct Char<0> {
	operator string() { return "!'_'"; }
};

template <> struct Char<1> {
	operator string() { return "'!'-' '"; }
};

template <unsigned int n> struct Char {
	operator string() {
		if (n & 1) {
			return "((" + string(Char<(n>>1)>()) + ")<<(" + string(Char<1>()) + "))|" + string(Char<1>());
		}
		return "(" + string(Char<(n>>1)>()) + ")<<(" + string(Char<1>()) + ")";
	}
};

int main(int argc, char* argv[])
{
	cout << "_[]={" << endl;
#include "chars.cc"
	cout << "};" << endl;
	return 0;
}
