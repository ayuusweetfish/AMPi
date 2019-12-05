#ifndef _ampi_env_h
#define _ampi_env_h

#ifdef __cplusplus
extern "C" {
#endif

// == Configuration ==

// If RASPPI == 1 and GPU_L2_CACHE_ENABLED; otherwise use 0xc0000000
// See circle/bcm2835.h
#define GPU_MEM_BASE 0x40000000

// Replace with #define to turn on multi-core support
#undef ARM_ALLOW_MULTI_CORE

// == Environment functions ==

#include <stddef.h>
#include <stdint.h>

// -- Memory allocation --
void *ampi_malloc (size_t size);
void ampi_free (void *ptr);

// -- Assertion --
void ampi_assertion_failed (const char *pExpr, const char *pFile, unsigned nLine);

// -- Co-operative scheduler --
// Creates a thread and returns its ID. The thread should start
// its execution soon.
int SchedulerCreateThread (int (*fn) (void *), void *param);
// Called once, the function pointer passed here should be called
// whenever a context switch takes place, with the argument being
// the ID of the thread being switched to.
void SchedulerRegisterSwitchHandler (void (*fn) (int));
// Gives up control and switch to another thread.
void SchedulerYield ();

// -- Timer --
// Delays
void MsDelay (unsigned nMilliSeconds);
void usDelay (unsigned nMicroSeconds);

// Called once, the handler passed here should be called with a
// fixed frequency. 100 Hz is a reasonable value.
typedef void TPeriodicTimerHandler (void);
void RegisterPeriodicHandler (TPeriodicTimerHandler *pHandler);

// -- Interrupts --
// Called once with nIRQ being 66 (ARM doorbell 0 -- see the
// BCMxxxx ARM peripherals datasheet).
typedef void TInterruptHandler (void *pParam);
void ConnectInterrupt (unsigned nIRQ, TInterruptHandler *pHandler, void *pParam);

// -- Mailbox --
// Use property tag 0x48010 with a size of 32 bits, pass `buf` as
// the argument and return the response value as a 32-bit integer.
uint32_t EnableVCHIQ (uint32_t buf);

// -- Logging --
#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_NOTICE  3
#define LOG_DEBUG   4
void LogWrite (const char *pSource,         // short name of module
               unsigned    Severity,        // see above
               const char *pMessage, ...);  // uses printf format options

// -- Miscellaneous --
// Returns a pointer to a 512KiB region configured to be Strongly Ordered.
// For ARMv6, see the Architectural Reference Manual, p. B4-12.
void *GetCoherentRegion512K ();

#ifdef __cplusplus
}
#endif

#endif
