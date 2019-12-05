#ifndef _ampi_env_h
#define _ampi_env_h

#ifdef __cplusplus
extern "C" {
#endif

// == Configuration ==

// If RASPPI == 1 and GPU_L2_CACHE_ENABLED; otherwise use 0xc0000000
// See circle/bcm2835.h
#define GPU_MEM_BASE 0x40000000

#undef ARM_ALLOW_MULTI_CORE

#define PAGE_SIZE 4096

// == Environment functions ==

#include <stddef.h>
#include <stdint.h>

void *ampi_malloc (size_t size);
void ampi_free (void *ptr);

void ampi_assertion_failed (const char *pExpr, const char *pFile, unsigned nLine);

int SchedulerCreateThread (int (*fn) (void *), void *param);
void SchedulerRegisterSwitchHandler (void (*fn) (int));
void SchedulerYield ();

void MsDelay (unsigned nMilliSeconds);
void usDelay (unsigned nMicroSeconds);

typedef void TPeriodicTimerHandler (void);
void RegisterPeriodicHandler (TPeriodicTimerHandler *pHandler);

typedef void TInterruptHandler (void *pParam);
void ConnectInterrupt (unsigned nIRQ, TInterruptHandler *pHandler, void *pParam);

uint32_t EnableVCHIQ (uint32_t buf);

#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_NOTICE  3
#define LOG_DEBUG   4

void LogWrite (const char *pSource,         // short name of module
               unsigned    Severity,        // see above
               const char *pMessage, ...);  // uses printf format options

void *GetCoherentRegion512K ();

#ifdef __cplusplus
}
#endif

#endif
