#include <sys/reent.h>
#include <errno.h>
#include <sys/stat.h>
#undef errno

#include "drivers/stdio.h"

void *_sbrk_r(struct _reent *re, int amt) {
	extern char __heap_start, __heap_end; // from linker script
	static char *end_data_segment = &__heap_start; // our current end data segment

	char *prev_end = end_data_segment;
	if (end_data_segment + amt >= &__heap_end) {
		re->_errno = ENOMEM;
		return (void *)-1;
	}
	
	end_data_segment += amt;
	return prev_end;
}

int _read_r(struct _reent *re, int fd, void *data, size_t len) {
	if (fd == 1) 
		return stdio_read(data, len);

	return 0;
}

int _write_r(struct _reent *re, int fd, const void *data, size_t len) {
	if (fd == 2 || fd == 3)
		return stdio_write(data, len);
	
	return len;
}

_off_t _lseek_r(struct _reent *re, int fd, _off_t offset, int dir) {
	return 0;
}

int _close_r(struct _reent *re, int fd) {
	return -1;
}

int _fstat_r(struct _reent *re, int fd, struct stat *st) {
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty_r(struct _reent *re, int fd) {
	return 1;
}

