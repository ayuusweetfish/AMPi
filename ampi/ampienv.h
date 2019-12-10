#ifndef _ampi_env_h
#define _ampi_env_h

#ifdef __cplusplus
extern "C" {
#endif

// == Configuration ==

// If RASPPI == 1 and GPU_L2_CACHE_ENABLED; otherwise use 0xc0000000
// Reference: circle/bcm2835.h
#define GPU_MEM_BASE 0x40000000

// Replace with #define to turn on multi-core support
#undef ARM_ALLOW_MULTI_CORE

// == Environment functions ==

#include <stddef.h>
#include <stdint.h>

// -- Memory allocation --
// On ARMv6/v7-A the returned memory should be configured to have
// Normal attribute, since ldrex/strex will be executed upon them.
// For reference, a total of 1392 bytes will be allocated for a
// single initialization on a Pi Zero. Starting and stopping do
// not incur memory allocations.
void *ampi_malloc (size_t size);
void ampi_free (void *ptr);

// -- Assertion --
void ampi_assertion_failed (const char *pExpr, const char *pFile, unsigned nLine);

// -- Timer --
// Delays
void MsDelay (unsigned nMilliSeconds);
void usDelay (unsigned nMicroSeconds);

// Called once. The handler passed here should be called with a
// fixed frequency. 100 Hz is a reasonable value.
typedef void TPeriodicTimerHandler (void);
void RegisterPeriodicHandler (TPeriodicTimerHandler *pHandler);

// -- Interrupts --
// Called once with nIRQ being 66 (ARM doorbell 0 -- see the
// BCMxxxx ARM peripherals datasheet).
typedef void TInterruptHandler (void *pParam);
void ConnectInterrupt (unsigned nIRQ, TInterruptHandler *pHandler, void *pParam);

// -- Mailbox --
// Use property tag 0x48010 with a size of 32 bits, pass `p` as
// the argument and return the response value as a 32-bit integer.
uint32_t EnableVCHIQ (uint32_t p);

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
void *GetCoherentRegion512K ();

#ifdef __cplusplus
}
#endif

#endif
