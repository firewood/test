#include <stdarg.h>

void unknown_function();

int int_args(int a, int b, int c, int d, int e, int f, int g, int h, int i, int j, int k, int l) {
	return a;
}

float float_args(int a, int b, float c, float d, float e, float f, float g, float h, double i, double j, double k, double l) {
	return c * d;
}

int var_args_int(int a, int b, ...) {
	va_list args;
	va_start(args, b);
	int c = va_arg(args, int);
	int d = va_arg(args, int);
	int e = va_arg(args, int);
	int f = va_arg(args, int);
	va_end(args);
	return c + d + e * f;
}

int var_args(float x, int a, int b, ...) {
	va_list args;
	va_start(args, b);
	int c = va_arg(args, int);
	int d = va_arg(args, int);
	int e = va_arg(args, int);
	int f = va_arg(args, int);
	va_end(args);
	return c + d + e * f;
}

void test_int_args() {
  int_args('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l');
}

void test_float_args() {
  float_args('a', 'b', 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0);
}

void test_var_args_int() {
  var_args('a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l');
}

void test_var_args_float() {
  var_args(1.0, 'a', 'b', 2.0, 3, 4.0, 5, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0, 12.0);
}

void test_unknown_function() {
	unknown_function(1, 2.0, 3, 4.0, 5, 6.0);
}

