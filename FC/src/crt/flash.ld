OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(flash_vector)

MEMORY {
	flash (rw) : org = 0x00000000, len = 128k
	sram (rwx) : org = 0x20000000, len = 20k
}

MAIN_STACK_SIZE = 6k;

SECTIONS {
	.flash_vector : {
		*(.flash_vector)
	} > flash

	.text : {
		*(.text .text*)
		*(.rodata .rodata*)
	} >flash
	
	.data : {
		__data_load = LOADADDR(.data);
		
		. = ALIGN(4);
		__data_start = .;
		
		*(.data .data*)
		
		. = ALIGN(4);
		__data_end = .;
	} >sram AT>flash
	
	.bss : {
		. = ALIGN(4);
		__bss_start = .;
		
		*(.bss .bss*)
		
		. = ALIGN(4);
		__bss_end = .;
	} >sram
	
	.stack : {
		. = ALIGN(4);
		. += MAIN_STACK_SIZE;
		
		. = ALIGN(4);
		__main_stack_top = .;
	} >sram
	
	.heap : {
		. = ALIGN(4);
		__heap_start = .;
		
		. += (0x20000000 + 20k - 32) - ABSOLUTE(.);
		__heap_low = .;
		
		. += (0x20000000 + 20k) - ABSOLUTE(.);
		__heap_end = .;
	} > sram
}

