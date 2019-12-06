#ifndef __coroutine_h
#define __coroutine_h

#ifdef __cplusplus
extern "C" {
#endif

// Implementation of a simple co-operative scheduler.

// co_next() may only be called on the main thread and
// executes a thread until it calls co_yield().

// co_callback() sets a callback called whenever a task
// switch happens, with the argument being the ID of
// the thread being switched to, or 0 if yielding from
// a thread.

// If yield() is called on the main thread, co_next()
// will be called for each active thread instead.

#include <stdint.h>

#define MAX_CO      8

int8_t co_create(void (*fn)(void *), void *arg);
void co_yield();
void co_next(int8_t id);
void co_callback(void (*cb)(int8_t));

#ifdef __cplusplus
}
#endif

#endif
