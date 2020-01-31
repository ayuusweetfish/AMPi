#ifndef _ampi_h
#define _ampi_h

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialization
bool AMPiInitialize(unsigned nSampleRate, unsigned nChunkSize);

// The callback produces 16-bit interleaved stereo sample data.
// It should write `nChunkSize` elements to the array `pBuffer`,
// that is, half that many stereo samples.
// It should return the number of elements written. If the value
// returned is less than `nChunkSize`, playback will automatically
// be stopped.
typedef unsigned (*chunk_cb_t) (int16_t **pBuffer, unsigned nChunkSize);
void AMPiSetChunkCallback(chunk_cb_t Callback);

// As the names suggest
bool AMPiStart();
bool AMPiStop();
bool AMPiIsActive();

// Call this periodically to keep audio running. Frequencies near
// or above 100 Hz should cause no problem, but feel free to try
// any other value.
// This can be done either from a loop in the main thread, or from
// an interrupt handler for a timer.
void AMPiPoke();

#ifdef __cplusplus
}
#endif

#endif
