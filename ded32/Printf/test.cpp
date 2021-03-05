extern "C" size_t aprintf(const char *fmt, ...);

extern "C" unsigned testCppCallback(unsigned a, unsigned b) {
	aprintf("// Called testCppCallback(0x%x, 0x%x);\n", a, b);
	
	return a + b;
}

int main() {
	aprintf("Hi?..\nYes, hi indeed!\nThis is %%d: %d\nThis is %%s: %s\n%%d vs %%u: %d, %u\n", 123, "YES, I AM!", -123, -123);
	
	return 0;
}