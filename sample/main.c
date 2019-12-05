#include <ampi.h>

#include "coroutine.h"
#include "common.h"
#include "circle/synchronize.h"

#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

CVCHIQDevice		    m_VCHIQ;
CVCHIQSoundBaseDevice   m_VCHIQSound;

void InitializeMMU (void);
void _irq_stub ();

unsigned synth(int16_t *buf, unsigned chunk_size);
void PlaybackThread (void *_unused);

int main (void)
{
	// MMU
	InitializeMMU ();

	// IRQs
	uint32_t *irq = (uint32_t *)0x18;
	*irq = 0xea000000 | ((uint32_t *)_irq_stub - irq - 2);
	__asm__ __volatile__ ("cpsie i");

	// Start the timer in the environment
	env_init();

	// Initialize device
	CVCHIQDevice_Initialize(&m_VCHIQ);
	CVCHIQSoundBaseDevice_Ctor(&m_VCHIQSound, &m_VCHIQ, 44100, 4000, VCHIQSoundDestinationAuto);

	// Set audio callback
	m_VCHIQSound.chunk_cb = synth;

	// Create the playback thread which waits until
	// the synth() function stops producing output,
	// waits 2 seconds and then restarts playback
	co_create(PlaybackThread, 0);

	// This loop acts as a scheduler
	while (1) {
		for (int i = 1; i <= 16; i++) co_next(i);
	}
}

unsigned synth(int16_t *buf, unsigned chunk_size)
{
	static uint8_t phase = 0;
	static uint32_t count = 0;
	if (count >= 131072) { count = 0; return 0; }
	for (unsigned i = 0; i < chunk_size; i += 2) {
		int16_t sample = (int16_t)(32767 * sin(phase / 255.0 * M_PI * 2));
		buf[i] = buf[i + 1] = sample;
		phase += 2; // Folds over to 0 ~ 255, generates 344.5 Hz (F4 - ~1/4 semitone)
	}
	count += (chunk_size >> 1);
	return chunk_size;
}

void PlaybackThread (void *_unused)
{
	while (1) {
		CVCHIQSoundBaseDevice_Start(&m_VCHIQSound);
		while (CVCHIQSoundBaseDevice_IsActive (&m_VCHIQSound)) co_yield();
		MsDelay (2000);
	}
}
