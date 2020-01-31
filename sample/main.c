#include <ampi.h>
#include <ampienv.h>

#include <linux/coroutine.h>
#include "common.h"

// Implemented in synth.c or synth_ogg.c
unsigned synth(int16_t **buf, unsigned chunk_size);

int main (void)
{
	// Start the timer in the environment
	env_init();

	// Initialize audio
	DSB();
	AMPiInitialize(44100, 4000);
	AMPiSetChunkCallback(synth);
	DMB();

	// Start playback
	while (1) {
		DSB();
		AMPiStart();
		while (AMPiIsActive()) { }
		DMB();

		DSB();
		MsDelay(2000);
		DMB();
	}
}
