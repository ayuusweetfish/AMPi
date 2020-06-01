#include <ampi.h>
#include <ampienv.h>

#include <linux/synchronize.h>
#include "common.h"

#include <stdarg.h>

void usDelay (unsigned nMicroSeconds)
{
	DMB(); DSB();
	uint32_t val = *SYSTMR_CLO + nMicroSeconds;
	while (*SYSTMR_CLO < val) { }
	DMB(); DSB();
}

void MsDelay (unsigned nMilliSeconds)
{
	usDelay(nMilliSeconds * 1000);
}

static TPeriodicTimerHandler *periodic = NULL;

void RegisterPeriodicHandler (TPeriodicTimerHandler *pHandler)
{
	periodic = pHandler;
}

// Interrupts

#define MAX_HANDLERS    128

static irq_handler handlers[MAX_HANDLERS] = { NULL };
static void *args[MAX_HANDLERS] = { NULL };

void set_irq_handler(uint8_t source, irq_handler f, void *arg)
{
	if (source < 0 || source >= MAX_HANDLERS) return;
	handlers[source] = f;
	args[source] = arg;
	DMB(); DSB();
	if (f) {
		if (source < 32) *INT_IRQENAB1 = (1 << source);
		else if (source < 64) *INT_IRQENAB2 = (1 << (source - 32));
		else if (source < 72) *INT_IRQBASENAB = (1 << (source - 64));
	} else {
		if (source < 32) *INT_IRQDISA1 = (1 << source);
		else if (source < 64) *INT_IRQDISA2 = (1 << (source - 32));
		else if (source < 72) *INT_IRQBASDISA = (1 << (source - 64));
	}
	DMB(); DSB();
}

void _int_irq()
{
	DMB(); DSB();

	// Check interrupt source
	uint32_t pend_base = *INT_IRQBASPEND;
	uint8_t source;
	if (pend_base & (1 << 8)) {
		source = 0 + __builtin_ctz(*INT_IRQPEND1);
	} else if (pend_base & (1 << 9)) {
		source = 32 + __builtin_ctz(*INT_IRQPEND2);
	} else if (pend_base & 0xff) {
		source = 64 + __builtin_ctz(pend_base & 0xff);
	} else {
		// Should not reach here
		DMB(); DSB();
		return;
	}

	if (handlers[source]) (*handlers[source])(args[source]);
	DMB(); DSB();
}

void ConnectInterrupt (unsigned nIRQ, TInterruptHandler *pHandler, void *pParam)
{
	set_irq_handler(nIRQ, pHandler, pParam);
}

#define T1_INTV	(1000000 / 100)

void timer1_handler(void *_unused)
{
	DMB(); DSB();
	uint32_t cur = *SYSTMR_CLO;
	uint32_t val = *SYSTMR_C1 + T1_INTV;
	if (val <= cur) val = cur + T1_INTV;
	*SYSTMR_C1 = val;
	*SYSTMR_CS = (1 << 1);
	DMB(); DSB();

	if (periodic) periodic();
	AMPiPoke();
}

void InitializeMMU (void);
void _irq_stub ();

void env_init()
{
	// MMU
	InitializeMMU ();

	// IRQs
	uint32_t *irq = (uint32_t *)0x18;
	*irq = 0xea000000 | ((uint32_t *)_irq_stub - irq - 2);
	__asm__ __volatile__ ("cpsie i");

	// Timer
	*SYSTMR_C1 = *SYSTMR_CLO + T1_INTV;
	ConnectInterrupt(1, timer1_handler, NULL);
}

uint32_t EnableVCHIQ (uint32_t buf)
{
	typedef mbox_buf(4) proptag;
	proptag *mboxbuf = (proptag *) 0x4c80000;
	mbox_init(*mboxbuf);
	mboxbuf->tag.id = 0x48010;
	mboxbuf->tag.u32[0] = buf;
	mbox_emit(*mboxbuf);
	return mboxbuf->tag.u32[0];
}

void LogWrite (const char *pSource,
               unsigned    Severity,
               const char *pMessage, ...)
{
	va_list var;
	va_start (var, pMessage);

	//CLogger::Get ()->WriteV (pSource, (TLogSeverity) Severity, pMessage, var);

	va_end (var);
}

void *GetCoherentRegion512K ()
{
	return (void *) 0x4c00000;
}

#ifndef AARCH64
	#define	EnableInterrupts()	__asm volatile ("cpsie i")
	#define	DisableInterrupts()	__asm volatile ("cpsid i")
#else
	#define	EnableInterrupts()	__asm volatile ("msr DAIFClr, #2")
	#define	DisableInterrupts()	__asm volatile ("msr DAIFSet, #2")
#endif

static volatile unsigned char irq_enab_save;

static void EnterCritical (void)
{
	DMB(); DSB();
#ifndef AARCH64
	uint32_t cpsr;
	asm volatile ("mrs %0, cpsr" : "=r" (cpsr));
#else
	uint64_t cpsr;
	asm volatile ("mrs %0, daif" : "=r" (cpsr));
#endif

	DisableInterrupts();
	irq_enab_save = (cpsr & 0x80) == 0;
	DMB(); DSB();
}

static void LeaveCritical (void)
{
	DMB(); DSB();
	if (irq_enab_save) EnableInterrupts();
	DMB(); DSB();
}

static char pool[1048576 * 32];
static volatile size_t ptr = 0;

void *ampi_malloc (size_t size)
{
	EnterCritical();
	volatile size_t p = ptr;
	ptr = p + size;
	LeaveCritical();
	void *ret = (void *)(pool + p);
	return ret;
}

void ampi_free (void *ptr)
{
}

void ampi_assertion_failed (const char *pExpr, const char *pFile, unsigned nLine)
{
}

// Mailbox, used internally by this environment implementation
void send_mail(uint32_t data, uint8_t channel)
{
    DMB(); DSB();
    while ((*MAIL0_STATUS) & (1u << 31)) { }
    *MAIL0_WRITE = (data << 4) | (channel & 15);
    DMB(); DSB();
}

uint32_t recv_mail(uint8_t channel)
{
    DMB(); DSB();
    do {
        while ((*MAIL0_STATUS) & (1u << 30)) { }
        uint32_t data = *MAIL0_READ;
        if ((data & 15) == channel) {
            DMB();
            return (data >> 4);
        //} else {
        //    printf("Incorrect channel (expected %u got %u)\n", channel, data & 15);
        }
    } while (1);
}
