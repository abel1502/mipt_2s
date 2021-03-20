extern "C" size_t aprintf(const char *fmt, ...);

extern "C" unsigned testCppCallback(unsigned a, unsigned b) {
	aprintf("// Called testCppCallback(0x%x, 0x%x);\n", a, b);
	
	return a + b;
}

int main() {
	unsigned written = 0;
	aprintf("Hi?..\nYes, hi indeed!\nThis is %%d: %d\n%nThis is %%s: %s\n%%d vs %%u: %d, %u\n", 123, &written, "YES, I AM!", -123, -123);
	written = (unsigned)aprintf("By the way, the first printf wrote %u characters in the first three lines\n"
								"and I %s %x %d%%%c%b", written, "love", 3802, 100, 33, 255);
	aprintf(".\n");
	aprintf("That was %u characters, by the way.", written);
	aprintf("hi %m%an | smb[%c] str[%s], %d, %d, %d, %d, %d, %d, %d, %x, %b\n\nAnd I %s %x%d%%%c%b", '$', "alalalalala", testCppCallback(1, 10), 2, 3, 4, 5, 6, 7, -8, -9, "LOVE", 3802, 100, 33, 255);
	
	return 0;
}