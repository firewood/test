extern void dummy_1(int a);
extern void dummy_2(int a, ...);

void test1() {
  dummy_1(99);
}

void test2() {
  dummy_2(99, 100, 101);
}

