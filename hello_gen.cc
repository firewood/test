#include <iostream>
#include <string>

using namespace std;

template <unsigned int n> struct Symbolizer {
	string s;
	Symbolizer() {
		if (n >= 0x10000) {
			s = "(" + Symbolizer<(n >> 16)>().s + ")<<(" + Symbolizer<16>().s + ")";
			if (n & 0xffff) {
				s = Symbolizer<(n & 0xffff)>().s + "|(" + s + ")";
			}
		} else if (n >= 0x100) {
			s = "(" + Symbolizer<(n >> 8)>().s + ")<<(" + Symbolizer<8>().s + ")";
			if (n & 0xff) {
				s = Symbolizer<(n & 0xff)>().s + "|(" + s + ")";
			}
		} else if (n >= 0x10) {
			s = "(" + Symbolizer<(n >> 4)>().s + ")<<(" + Symbolizer<4>().s + ")";
			if (n & 0xf) {
				s = Symbolizer<(n & 0xf)>().s + "|(" + s + ")";
			}
		} else {
			s = "(" + Symbolizer<(n >> 1)>().s + ")<<(" + Symbolizer<1>().s + ")";
			if (n & 1) {
				s = Symbolizer<1>().s + "|(" + s + ")";
			}
		}
	}
};

template <> Symbolizer<0>::Symbolizer() { s = "!\"\""; }
template <> Symbolizer<1>::Symbolizer() { s = "!!\"\""; }
template <> Symbolizer<2>::Symbolizer() { s = "'#'-'!'"; }
template <> Symbolizer<3>::Symbolizer() { s = "'#'-' '"; }
template <> Symbolizer<4>::Symbolizer() { s = "'$'-' '"; }
template <> Symbolizer<5>::Symbolizer() { s = "'%'-' '"; }
template <> Symbolizer<6>::Symbolizer() { s = "'&'-' '"; }
template <> Symbolizer<7>::Symbolizer() { s = "'('-'!'"; }
template <> Symbolizer<8>::Symbolizer() { s = "'('-' '"; }
template <> Symbolizer<9>::Symbolizer() { s = "')'-' '"; }
template <> Symbolizer<10>::Symbolizer() { s = "'*'-' '"; }
template <> Symbolizer<11>::Symbolizer() { s = "'+'-' '"; }
template <> Symbolizer<12>::Symbolizer() { s = "','-' '"; }
template <> Symbolizer<13>::Symbolizer() { s = "'-'-' '"; }
template <> Symbolizer<14>::Symbolizer() { s = "'.'-' '"; }
template <> Symbolizer<15>::Symbolizer() { s = "'/'-' '"; }
template <> Symbolizer<16>::Symbolizer() { s = "'='-'-'"; }

int main(int argc, char* argv[])
{
	cout << "_[]={" << endl;
#include "numbers.cc"
	cout << "};" << endl;
	return 0;
}
