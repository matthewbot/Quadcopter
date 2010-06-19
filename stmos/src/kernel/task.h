#ifndef STMOS_KERNEL_TASK_H
#define STMOS_KERNEL_TASK_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

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

#define TASK_STACKGUARD_VALUE 0xABCD

struct kernel_task {
	struct kernel_task *list_next;
	struct kernel_task *list_prev;
	struct kernel_task *tasklist_next;
	struct kernel_task *tasklist_prev;
	void *sp;
	
	char name[16];
	kernel_taskpri pri;
	
	enum task_state state;
	bool needfree;	
	
	union {
		uint32_t num;
	} list_data;
	
	void *userdata;
	uint16_t stackguard;
};

inline bool task_checkstack(const struct kernel_task *task) { 
	return task->stackguard == TASK_STACKGUARD_VALUE;
}

#define task_assertstack(task) assert((task)->stackguard == TASK_STACKGUARD_VALUE)

typedef void (*kernel_taskfunc)(void *);

struct kernel_task *task_new(const char *name, kernel_taskpri pri, kernel_taskfunc func, void *data, size_t stacksize);
struct kernel_task *task_new_inplace(const char *name, kernel_taskpri pri, kernel_taskfunc func, void *data, char *buf, size_t bufsize);
void task_list_add(struct kernel_task *listpos, struct kernel_task *task);
struct kernel_task *task_list_remove(struct kernel_task *task);
void task_list_append_sorted(struct kernel_task *list, struct kernel_task *task);
void task_free(struct kernel_task *task);

void task_gc();

#endif
