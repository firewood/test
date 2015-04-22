#include <iostream>
#include <string>

using namespace std;

template <int n> struct Char;

template <> struct Char<0> {
	operator string() { return "!_"; }
};

template <> struct Char<1> {
	operator string() { return "_"; }
};

template <> struct Char<2> {
	operator string() { return "_+_"; }
};

template <> struct Char<3> {
	operator string() { return "_+_+_"; }
};

template <int n> struct Char {
	operator string() {
		if (n & 1) {
			return "((" + string(Char<(n>>1)>()) + ")<<_)|" + string(Char<1>());
		}
		return "(" + string(Char<(n>>1)>()) + ")<<_";
	}
};

int main(int argc, char* argv[])
{
	cout << string(Char<1>()) << endl;
	cout << string(Char<2>()) << endl;
	cout << string(Char<3>()) << endl;
	cout << string(Char<4>()) << endl;

	cout << string(Char<'H'>()) << endl;
	cout << string(Char<'e'>()) << endl;
	cout << string(Char<'l'>()) << endl;
	cout << string(Char<'l'>()) << endl;
	cout << string(Char<'o'>()) << endl;
}
