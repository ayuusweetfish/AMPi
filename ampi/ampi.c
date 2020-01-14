#include "ampi.h"
#include "ampienv.h"

#include <vc4/vchiq/vchiqdevice.h>
#include <vc4/sound/vchiqsoundbasedevice.h>
#include <linux/coroutine.h>

static CVCHIQDevice m_VCHIQ;
static CVCHIQSoundBaseDevice m_VCHIQSound;

__attribute__((visibility("default")))
bool AMPiInitialize(unsigned nSampleRate, unsigned nChunkSize)
{
    if (!CVCHIQDevice_Initialize(&m_VCHIQ)) return false;

    CVCHIQSoundBaseDevice_Ctor(&m_VCHIQSound, &m_VCHIQ,
        nSampleRate, nChunkSize, VCHIQSoundDestinationAuto);
    return true;
}

__attribute__((visibility("default")))
void AMPiSetChunkCallback(chunk_cb_t Callback)
{
    m_VCHIQSound.ChunkCallback = Callback;
}

__attribute__((visibility("default")))
bool AMPiStart()
{
    return CVCHIQSoundBaseDevice_Start(&m_VCHIQSound);
}

__attribute__((visibility("default")))
bool AMPiStop()
{
    return CVCHIQSoundBaseDevice_Cancel(&m_VCHIQSound);
}

__attribute__((visibility("default")))
bool AMPiIsActive()
{
    return CVCHIQSoundBaseDevice_IsActive(&m_VCHIQSound);
}

__attribute__((visibility("default")))
void AMPiPoke()
{
    for (int i = 1; i <= MAX_CO; i++) ampi_co_next(i);
}
