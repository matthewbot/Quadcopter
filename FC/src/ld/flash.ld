OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(g_pfnVectors)

MEMORY {
	flash (rw) : org = 0x80000000, len = 128k
	sram (rwx) : org = 0x20000000, len = 20k
}

MAIN_STACK_SIZE = 6k;

SECTIONS {
	.text : {
		*(.isr_vector)
		*(.text .text*)
		*(.rodata .rodata*)
		
		. = ALIGN(4);
		_etext = .;
	} >flash
	
	.data : {
		_sidata = LOADADDR(.data);
		
		. = ALIGN(4);
		_sdata = .;
		
		*(.data .data*)
		
		. = ALIGN(4);
		_edata = .;
	} >sram AT>flash
	
	.bss : {
		. = ALIGN(4);
		_sbss = .;
		
		*(.bss .bss*)
		
		. = ALIGN(4);
		_ebss = .;
	} >sram
	
	.stack : {
		. = ALIGN(4);
		. += MAIN_STACK_SIZE;
		
		. = ALIGN(4);
		_estack = .;
	} >sram
	
	.heap : {
		. = ALIGN(4);
		_sheap = .;
		
		. += (0x20000000 + 20k) - ABSOLUTE(.);
		_eheap = .;
	} > sram
}

