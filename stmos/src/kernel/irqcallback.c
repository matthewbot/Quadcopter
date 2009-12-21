#include "irqcallback.h"
#include "atomic.h"
#include <stmos/crt/panic.h>
#include <stddef.h>

struct irqcallback_entry {
	irqcallback callback;
	void *data;
};

#define MAX_IRQ_CALLBACKS 5
volatile static struct irqcallback_entry entries[MAX_IRQ_CALLBACKS];
volatile bool entries_nonempty;

void irqcallback_new(irqcallback callback, void *data) {
	entries_nonempty = true;

	int i;
	for (i=0; i < MAX_IRQ_CALLBACKS; i++) {
		volatile struct irqcallback_entry *entry = &entries[i];
		
		// attempt to atomically change the callback from NULL to our callback
		if (cmpxchg((volatile uint32_t *)&entry->callback, (uint32_t)NULL, (uint32_t)callback)) {
			// if we succeeded (the value was NULL, and we succesfully changed it to our value before anyone else)
			entry->data = data; // store our data as well
			return;
		}
	}
	
	panic("out of irqcallback slots");
}

void irqcallback_run() {
	if (!entries_nonempty)
		return;
	entries_nonempty = false;
	
	int i;
	for (i=0; i < MAX_IRQ_CALLBACKS; i++) {
		irqcallback callback = entries[i].callback;
		if (!callback)
			continue;
		callback(entries[i].data); // from here on we're safe, irqcallback_new doesn't change any non-null entries
		entries[i].callback = NULL; // when we set it NULL, its now up for grabs again
	}
}

