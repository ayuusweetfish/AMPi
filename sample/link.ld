/*
 * circle.ld
 */

ENTRY(_start)

SECTIONS
{
	.init : {
		*(.init)
	}

	.text : {
		*(.text*)

		_etext = .;
	}

	.rodata : {
		*(.rodata*)
	}

	.ARM.exidx : {
		__exidx_start = .;

		*(.ARM.exidx*)

		__exidx_end = .;
	}

	.data : {
		*(.data*)
	}

	.bss : {
		__bss_start = .;

		*(.bss*)
		*(COMMON)

		_end = .;
	}
}
