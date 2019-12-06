#include <ampi.h>

#include "coroutine.h"
#include "common.h"

unsigned synth(int16_t *buf, unsigned chunk_size);
void PlaybackThread (void *_unused);

int main (void)
{
	// Start the timer in the environment
	env_init();

	// Initialize audio
	AMPiInitialize(44100, 4000);
	AMPiSetChunkCallback(synth);

	// Create the playback thread which waits until
	// the synth() function stops producing output,
	// waits 2 seconds and then restarts playback
	co_create(PlaybackThread, 0);

	// This loop acts as a scheduler
	while (1) {
		for (int i = 1; i <= 16; i++) co_next(i);
	}
}

void PlaybackThread (void *_unused)
{
	while (1) {
		AMPiStart();
		while (AMPiIsActive()) co_yield();
		MsDelay (2000);
	}
}
