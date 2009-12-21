OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(flash_vector)
INPUT(crti.o crtn.o crtbegin.o crtend.o)
GROUP(-lgcc -lc -lm -lstmos)

MEMORY {
	flash (rw) : org = 0x00000000, len = 128k
	sram (rwx) : org = 0x20000000, len = 20k
}

STACK_SIZE = 1k;

SECTIONS {
	.flash_vector : {
		*(.flash_vector)
	} > flash

	.text : {
		CREATE_OBJECT_SYMBOLS
		*(.text .text*)
		*(.rodata .rodata*)
		
		. = ALIGN(0x4);
		__ctors_start = .;
		KEEP (*(EXCLUDE_FILE (*crtend.o *crtbegin.o) .ctors))
		KEEP (*(SORT(.ctors.*)))
		__ctors_end = .;
		*(.init)
	} > flash
	
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
		
		*(.bss .bss* COMMON)
		
		. = ALIGN(4);
		__bss_end = .;
	} >sram
	
	.stack : {
		__stack_start = .;
		. = ALIGN(4);
		. += STACK_SIZE;
		
		__stack_end = .;
	} >sram
	
	.heap : {
		. = ALIGN(4);
		__heap_start = .;
		
		. += (0x20000000 + 20k) - ABSOLUTE(.);
		__heap_end = .;
	} > sram
}

