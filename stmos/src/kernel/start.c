#include "task.h"
#include "sched.h"
#include "handler.h"
#include "irq.h"
#include <stdbool.h>
#include <stdint.h>

static void maintaskfunc();
static void idletaskfunc();

static struct kernel_task *maintask;

__attribute__((noreturn))
void kernel_start() {
	struct kernel_task *idletask = task_new("idle", KERNEL_PRIORITY_IDLE, idletaskfunc, 128, NULL);
	maintask = task_new("main", KERNEL_PRIORITY_MID, maintaskfunc, 1024*7, NULL);
	sched_add_task(idletask);
	sched_add_task(maintask);
	irq_setup();

	handler_begin();
}

extern int main(int argc, char **argv);

static void maintaskfunc(void *unused) {
	main(0, NULL);

	irq_disable_switch();
	sched_remove_task(maintask);
	task_free(maintask);
	irq_force_switch();
	irq_enable_switch();
}

__attribute__((noreturn))
static void idletaskfunc(void *unused) {
	while (true) {
		task_gc();
		asm volatile("wfi");
	}
}
