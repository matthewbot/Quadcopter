#include <stdint.h>
#include <sys/reent.h>
#include <errno.h>
#include <sys/stat.h>
#undef errno

void *_sbrk_r(struct _reent *re, int amt) {
	extern char _sheap, _eheap; // from linker script
	static char *end_data_segment = &_sheap; // our current end data segment

	char *prev_end = end_data_segment;
	if (end_data_segment + amt >= &_eheap) {
		re->_errno = ENOMEM;
		return (void *)-1;
	}
	
	end_data_segment += amt;
	return prev_end;
}

int _read_r(struct _reent *re, int fd, void *data, size_t len) {
	return 0;
}

int _write_r(struct _reent *re, int fd, const void *data, size_t len) {
	// TODO
	
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

void _exit(int val) {
	while (1) { }
}

int _kill_r(struct _reent *re, int pid, int sig) {
	return -1;
}

int _getpid_r(struct _reent *re) {
	return 1;
}

void assert_failed(uint8_t* file, uint32_t line) {
	while (1) { }
}

