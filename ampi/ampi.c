#include "ampi.h"
#include "ampienv.h"

#include <vc4/vchiq/vchiqdevice.h>
#include <vc4/sound/vchiqsoundbasedevice.h>

static CVCHIQDevice m_VCHIQ;
static CVCHIQSoundBaseDevice m_VCHIQSound;

bool AMPiInitialize(unsigned nSampleRate, unsigned nChunkSize)
{
    if (!CVCHIQDevice_Initialize(&m_VCHIQ)) return false;

    CVCHIQSoundBaseDevice_Ctor(&m_VCHIQSound, &m_VCHIQ,
        nSampleRate, nChunkSize, VCHIQSoundDestinationAuto);
    return true;
}

void AMPiSetChunkCallback(chunk_cb_t Callback)
{
    m_VCHIQSound.ChunkCallback = Callback;
}

bool AMPiStart()
{
    return CVCHIQSoundBaseDevice_Start(&m_VCHIQSound);
}

bool AMPiStop()
{
    return CVCHIQSoundBaseDevice_Cancel(&m_VCHIQSound);
}

bool AMPiIsActive()
{
    return CVCHIQSoundBaseDevice_IsActive(&m_VCHIQSound);
}

void AMPiPoke()
{
    // TODO
}
