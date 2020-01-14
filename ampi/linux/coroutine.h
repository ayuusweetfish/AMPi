#ifndef __coroutine_h
#define __coroutine_h

#ifdef __cplusplus
extern "C" {
#endif

// Implementation of a simple co-operative scheduler.

// ampi_co_next() may only be called on the main thread and
// executes a thread until it calls ampi_co_yield().

// ampi_co_callback() sets a callback called whenever a task
// switch happens, with the argument being the ID of the
// thread being switched to, or 0 if yielding from a thread.

// If yield() is called on the main thread, ampi_co_next()
// will be called for each active thread instead.

#include <stdint.h>

#define MAX_CO      8

int8_t ampi_co_create(void (*fn)(void *), void *arg);
void ampi_co_yield();
void ampi_co_next(int8_t id);
void ampi_co_callback(void (*cb)(int8_t));

#ifdef __cplusplus
}
#endif

#endif
