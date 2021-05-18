#include <cstdio>
#include <cassert>


extern "C" void solveSquare(double a, double b, double c);

extern "C" unsigned callback(unsigned i) {
	printf("Woah, a callback..\n");
	
	return i + 1;
}

int main() {
	printf("We're in c, by the way\n");
	
	double a = 0, b = 0, c = 0;
	int res = scanf("%lg %lg %lg", &a, &b, &c);
	assert(res == 3);
	
	solveSquare(a, b, c);
	
	printf("(Still are)\n");
	
	return 0;
}

