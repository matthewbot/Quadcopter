#include "linker.h"

void __run_ctors() {
	cdtor *ptr;
	for (ptr = &__ctors_start; ptr != &__ctors_end; ptr++) {
		(*ptr)();
	}
}

