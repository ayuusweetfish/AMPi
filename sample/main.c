#include <ampi.h>

#include "coroutine.h"
#include "common.h"
#include "circle/synchronize.h"

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

void PlaybackThread (void *_unused)
{
	while (1) {
		CVCHIQSoundBaseDevice_Start(&m_VCHIQSound);
		while (CVCHIQSoundBaseDevice_IsActive (&m_VCHIQSound)) co_yield();
		MsDelay (2000);
	}
}
