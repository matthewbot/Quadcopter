#include "task.h"
#include "sched.h"
#include "handler.h"
#include "irq.h"
#include "mutex.h"
#include <stdbool.h>
#include <stdint.h>

static void maintaskfunc();
static void idletaskfunc();

static struct kernel_task *maintask;
static struct kernel_mutex malloc_mutex;

static char idletask_mem[256];

__attribute__((noreturn))
void kernel_start() {
	mutex_init(&malloc_mutex);
	struct kernel_task *idletask = task_new_inplace("idle", KERNEL_PRIORITY_IDLE, idletaskfunc, NULL, idletask_mem, sizeof(idletask_mem));
	maintask = task_new("main", KERNEL_PRIORITY_MID, maintaskfunc, NULL, 1024*4);
	sched_add_task(idletask);
	sched_add_task(maintask);
	irq_setup();

	handler_begin();
}

extern int main(int argc, char **argv);
extern void __run_ctors();

__attribute__((noreturn))
static void maintaskfunc(void *unused) {
	__run_ctors();
	main(0, NULL);

	irq_disable_switch();
	sched_remove_task(maintask);
	task_free(maintask);
	irq_force_switch();
	irq_enable_switch();
	while (true) { }
}

__attribute__((noreturn))
static void idletaskfunc(void *unused) {
	while (true) {
		task_gc();
		asm volatile("wfi");
	}
}

void __malloc_lock() {
	if (sched_get_current_task() != NULL)
		mutex_wait(&malloc_mutex);
}

void __malloc_unlock() {
	if (sched_get_current_task() != NULL)
		mutex_release(&malloc_mutex);
}
