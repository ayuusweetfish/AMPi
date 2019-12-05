#include "circle/synchronize.h"

static void vfpinit (void)
{
	// Coprocessor Access Control Register
	unsigned nCACR;
	__asm volatile ("mrc p15, 0, %0, c1, c0, 2" : "=r" (nCACR));
	nCACR |= 3 << 20;	// cp10 (single precision)
	nCACR |= 3 << 22;	// cp11 (double precision)
	__asm volatile ("mcr p15, 0, %0, c1, c0, 2" : : "r" (nCACR));
	InstructionMemBarrier ();

#define VFP_FPEXC_EN	(1 << 30)
	__asm volatile ("fmxr fpexc, %0" : : "r" (VFP_FPEXC_EN));

#define VFP_FPSCR_DN	(1 << 25)	// enable Default NaN mode
	__asm volatile ("fmxr fpscr, %0" : : "r" (VFP_FPSCR_DN));
}

void sysinit (void)
{
	EnableFIQs ();		// go to IRQ_LEVEL, EnterCritical() will not work otherwise

	vfpinit ();

	// clear BSS
	extern unsigned char __bss_start;
	extern unsigned char _end;
	for (unsigned char *pBSS = &__bss_start; pBSS < &_end; pBSS++)
	{
		*pBSS = 0;
	}

	extern int main (void);
	main ();
	while (1) { }
}

#define MEM_PAGE_TABLE1 0x128000    // Must be 16K aligned

// stub.S
void EnableMMU (void *base_address);

void InitializeMMU (void)
{
	uint32_t *m_pTable = (uint32_t *)MEM_PAGE_TABLE1;

	for (unsigned nEntry = 0; nEntry < 4096; nEntry++)
	{
		u32 nBaseAddress = nEntry << 20;

		// Fault
		u32 nAttributes = 0;

		extern u8 _etext;
		if (nBaseAddress < (u32) &_etext)
		{
			// Normal
			nAttributes = 0x0040E;
		}
		else if (nBaseAddress == 0x1c00000)
		{
			// Strongly ordered
			nAttributes = 0x10412;
		}
		else if (nBaseAddress < 256 * 1024 * 1024)
		{
			// Normal, no execution
			nAttributes = 0x0041E;
		}
		else if (nBaseAddress < 0x20FFFFFF)
		{
			// Shared device
			nAttributes = 0x10416;
		}

		m_pTable[nEntry] = nBaseAddress | nAttributes;
	}

	CleanDataCache ();

	EnableMMU((uint32_t *)MEM_PAGE_TABLE1);
}
