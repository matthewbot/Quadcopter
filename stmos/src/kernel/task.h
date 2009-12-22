#ifndef STMOS_KERNEL_TASK_H
#define STMOS_KERNEL_TASK_H

#include <stddef.h>
#include <stdint.h>

typedef uint8_t kernel_taskpri;

#define KERNEL_PRIORITY_IDLE 0xFF
#define KERNEL_PRIORITY_LOWEST 0xFE // FF reserved for idle task
#define KERNEL_PRIORITY_LOW 0xC0
#define KERNEL_PRIORITY_MID 0x80
#define KERNEL_PRIORITY_HIGH 0x40
#define KERNEL_PRIORITY_HIGHEST 0x00

enum task_state {
	TASK_STATE_NONE,
	TASK_STATE_SCHEDULED,
	TASK_STATE_SLEEP,
};

struct kernel_tasklist_node {
	struct kernel_task *prev;
	struct kernel_task *next;
};

#define TASK_STACKGUARD_VALUE 0xABCD

struct kernel_task {
	struct kernel_tasklist_node listnode;
	void *sp;
	
	char name[11];
	kernel_taskpri pri;
	enum task_state state;
	
	union {
		uint32_t num;
		struct {
			uint32_t timeout;
			const volatile uint8_t *ptr;
			uint8_t val;
		} flag; 
	} list_data;
	
	void *userdata;
	uint16_t stackguard;
};

typedef void (*kernel_taskfunc)(void *);

struct kernel_task *task_new(const char *name, kernel_taskpri pri, kernel_taskfunc func, size_t stacksize, void *data);
void task_list_add(struct kernel_task *listpos, struct kernel_task *task);
struct kernel_task *task_list_remove(struct kernel_task *task);
void task_free(struct kernel_task *task);

void task_gc();

#endif