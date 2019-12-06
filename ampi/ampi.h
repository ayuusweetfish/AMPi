#ifndef _ampi_h
#define _ampi_h

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

bool AMPiInitialize(unsigned nSampleRate, unsigned nChunkSize);

typedef unsigned (*chunk_cb_t) (int16_t *pBuffer, unsigned nChunkSize);
void AMPiSetChunkCallback(chunk_cb_t Callback);

bool AMPiStart();
bool AMPiStop();
bool AMPiIsActive();

void AMPiPoke();

#ifdef __cplusplus
}
#endif

#endif
