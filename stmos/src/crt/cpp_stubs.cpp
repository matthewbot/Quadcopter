#include <stdlib.h>

extern "C" {
	#include "panic.h"
}

const void *const __dso_handle = (void*) &__dso_handle;

void *operator new(size_t size) throw() { 
	void *data = malloc(size);
	if (!data)
		panic("new: out of memory");
	return data;
}
void *operator new[](size_t size) throw() { return operator new(size); }
void operator delete(void *p) throw() { free(p); }
void operator delete[](void *p) throw() { free(p); }
extern "C" int __aeabi_atexit(void *object, void (*destructor)(void *), void *dso_handle) { return 0; }

