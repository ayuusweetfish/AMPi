//
// vchiqsoundbasedevice.cpp
//
// Portions have been taken from the Linux VCHIQ ALSA sound driver which is:
//    Copyright 2011 Broadcom Corporation. All rights reserved.
//    Licensed under GPLv2
//
// Circle - A C++ bare metal environment for Raspberry Pi
// Copyright (C) 2017  R. Stange <rsta2@o2online.de>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#include <vc4/sound/vchiqsoundbasedevice.h>
#include <linux/assert.h>
#include <linux/coroutine.h>

#define LOG(...)

#define VOLUME_TO_CHIP(volume)        ((unsigned) -(((volume) << 8) / 100))

// protected and private functions
int CVCHIQSoundBaseDevice_QueueMessage (CVCHIQSoundBaseDevice *_this, VC_AUDIO_MSG_T *pMessage)
{
    vchi_service_use (_this->m_hService);

    int nResult = vchi_msg_queue (_this->m_hService, pMessage, sizeof *pMessage,
                      VCHI_FLAGS_BLOCK_UNTIL_QUEUED, 0);

    vchi_service_release (_this->m_hService);

    return nResult;
}

int CVCHIQSoundBaseDevice_CallMessage (CVCHIQSoundBaseDevice *_this, VC_AUDIO_MSG_T *pMessage)
{
    _this->m_Event = false;

    int nResult = CVCHIQSoundBaseDevice_QueueMessage (_this, pMessage);
    if (nResult == 0)
    {
        while (!_this->m_Event) ampi_co_yield ();
    }
    else
    {
        _this->m_nResult = nResult;
    }

    return _this->m_nResult;
}

int CVCHIQSoundBaseDevice_WriteChunk (CVCHIQSoundBaseDevice *_this)
{
    if (_this->ChunkCallback == 0) return 0;
    s16 *Buffer = 0;
    unsigned nWords = (*_this->ChunkCallback) (&Buffer, _this->m_nChunkSize);
    if (nWords == 0 || Buffer == 0)
    {
        _this->m_State = VCHIQSoundIdle;

        return 0;
    }

    unsigned nBytes = nWords * sizeof (s16);

    VC_AUDIO_MSG_T Msg;

    Msg.type = VC_AUDIO_MSG_TYPE_WRITE;
    Msg.u.write.count = nBytes;
    Msg.u.write.max_packet = 4000;
    Msg.u.write.cookie1 = VC_AUDIO_WRITE_COOKIE1;
    Msg.u.write.cookie2 = VC_AUDIO_WRITE_COOKIE2;
    Msg.u.write.silence = 0;

    int nResult = vchi_msg_queue (_this->m_hService, &Msg, sizeof Msg, VCHI_FLAGS_BLOCK_UNTIL_QUEUED, 0);
    if (nResult != 0)
    {
        return nResult;
    }

    _this->m_nWritePos += nBytes;

    u8 *pBuffer8 = (u8 *) Buffer;
    while (nBytes > 0)
    {
        unsigned nBytesToQueue =   nBytes <= Msg.u.write.max_packet
                     ? nBytes
                     : Msg.u.write.max_packet;

        nResult = vchi_msg_queue (_this->m_hService, pBuffer8, nBytesToQueue,
                      VCHI_FLAGS_BLOCK_UNTIL_QUEUED, 0);
        if (nResult != 0)
        {
            return nResult;
        }

        pBuffer8 += nBytesToQueue;
        nBytes -= nBytesToQueue;
    }

    return 0;
}

void CVCHIQSoundBaseDevice_Callback (CVCHIQSoundBaseDevice *_this, const VCHI_CALLBACK_REASON_T Reason, void *hMessage)
{
    if (Reason != VCHI_CALLBACK_MSG_AVAILABLE)
    {
        assert (0);
        return;
    }

    vchi_service_use (_this->m_hService);

    VC_AUDIO_MSG_T Msg;
    uint32_t nMsgLen;
    int nResult = vchi_msg_dequeue (_this->m_hService, &Msg, sizeof Msg, &nMsgLen, VCHI_FLAGS_NONE);
    if (nResult != 0)
    {
        vchi_service_release (_this->m_hService);

        _this->m_State = VCHIQSoundError;

        assert (0);
        return;
    }

    switch (Msg.type)
    {
    case VC_AUDIO_MSG_TYPE_RESULT:
        _this->m_nResult = Msg.u.result.success;
        _this->m_Event = true;
        break;

    case VC_AUDIO_MSG_TYPE_COMPLETE:
        if (   _this->m_State == VCHIQSoundIdle
            || _this->m_State == VCHIQSoundError)
        {
            break;
        }
        assert (_this->m_State >= VCHIQSoundRunning);

        if (   Msg.u.complete.cookie1 != VC_AUDIO_WRITE_COOKIE1
            || Msg.u.complete.cookie2 != VC_AUDIO_WRITE_COOKIE2)
        {
            _this->m_State = VCHIQSoundError;

            assert (0);
            break;
        }

        _this->m_nCompletePos += Msg.u.complete.count & 0x3FFFFFFF;

        // if there is no more than one chunk left queued
        if (_this->m_nWritePos-_this->m_nCompletePos <= _this->m_nChunkSize*sizeof (s16))
        {
            if (_this->m_State == VCHIQSoundCancelled)
            {
                _this->m_State = VCHIQSoundTerminating;

                break;
            }

            if (CVCHIQSoundBaseDevice_WriteChunk (_this) != 0)
            {
                assert (0);

                _this->m_State = VCHIQSoundError;
            }
        }
        break;

    default:
        assert (0);
        break;
    }

    vchi_service_release (_this->m_hService);
}

static void CVCHIQSoundBaseDevice_CallbackStub (
    void *pParam, const VCHI_CALLBACK_REASON_T Reason, void *hMessage)
{
    CVCHIQSoundBaseDevice *pThis = (CVCHIQSoundBaseDevice *) pParam;
    assert (pThis != 0);

    CVCHIQSoundBaseDevice_Callback (pThis, Reason, hMessage);
}

void CVCHIQSoundBaseDevice_Ctor (CVCHIQSoundBaseDevice *_this, CVCHIQDevice *pVCHIQDevice,
                          unsigned nSampleRate,
                          unsigned nChunkSize,
                          enum TVCHIQSoundDestination Destination)
{
    //assert (44100 <= nSampleRate && nSampleRate <= 48000);
    assert (Destination < VCHIQSoundDestinationUnknown);

    _this->ChunkCallback = 0;
    _this->m_nSampleRate = nSampleRate;
    _this->m_nChunkSize = nChunkSize;
    _this->m_Destination = Destination;
    _this->m_State = VCHIQSoundCreated;
    _this->m_VCHIInstance = 0;
    _this->m_hService = 0;

    //CDeviceNameService::Get ()->AddDevice ("sndvchiq", this, FALSE);
}

boolean CVCHIQSoundBaseDevice_Start (CVCHIQSoundBaseDevice *_this)
{
    if (_this->m_State > VCHIQSoundIdle)
    {
        return FALSE;
    }

    VC_AUDIO_MSG_T Msg;
    int nResult;

    if (_this->m_State == VCHIQSoundCreated)
    {
        nResult = vchi_initialise (&_this->m_VCHIInstance);
        if (nResult != 0)
        {
            LOG (FromVCHIQSound, LogError,
                        "Cannot initialize VCHI (%d)", nResult);

            _this->m_State = VCHIQSoundError;

            return FALSE;
        }

        nResult = vchi_connect (0, 0, _this->m_VCHIInstance);
        if (nResult != 0)
        {
            LOG (FromVCHIQSound, LogError,
                        "Cannot connect VCHI (%d)", nResult);

            _this->m_State = VCHIQSoundError;

            return FALSE;
        }

        SERVICE_CREATION_T Params =
        {
            VCHI_VERSION_EX (VC_AUDIOSERV_VER, VC_AUDIOSERV_MIN_VER),
            VC_AUDIO_SERVER_NAME,
            0, 0, 0,        // unused
            CVCHIQSoundBaseDevice_CallbackStub, _this,
            1, 1, 0            // unused (bulk)
        };

        nResult = vchi_service_open (_this->m_VCHIInstance, &Params, &_this->m_hService);
        if (nResult != 0)
        {
            LOG (FromVCHIQSound, LogError,
                        "Cannot open AUDS service (%d)", nResult);

            _this->m_State = VCHIQSoundError;

            return FALSE;
        }

        vchi_service_release (_this->m_hService);

        Msg.type = VC_AUDIO_MSG_TYPE_CONFIG;
        Msg.u.config.channels = 2;
        Msg.u.config.samplerate = _this->m_nSampleRate;
        Msg.u.config.bps = 16;

        nResult = CVCHIQSoundBaseDevice_CallMessage (_this, &Msg);
        if (nResult != 0)
        {
            LOG (FromVCHIQSound, LogError,
                        "Cannot set config (%d)", nResult);

            _this->m_State = VCHIQSoundError;

            return FALSE;
        }

        Msg.type = VC_AUDIO_MSG_TYPE_CONTROL;
        Msg.u.control.dest = _this->m_Destination;
        Msg.u.control.volume = VOLUME_TO_CHIP (VCHIQ_SOUND_VOLUME_DEFAULT);

        nResult = CVCHIQSoundBaseDevice_CallMessage (_this, &Msg);
        if (nResult != 0)
        {
            LOG (FromVCHIQSound, LogError,
                        "Cannot set control (%d)", nResult);

            _this->m_State = VCHIQSoundError;

            return FALSE;
        }

        Msg.type = VC_AUDIO_MSG_TYPE_OPEN;

        nResult = CVCHIQSoundBaseDevice_QueueMessage(_this, &Msg);
        if (nResult != 0)
        {
            LOG (FromVCHIQSound, LogError,
                        "Cannot open audio (%d)", nResult);

            _this->m_State = VCHIQSoundError;

            return FALSE;
        }

        _this->m_State = VCHIQSoundIdle;
    }

    assert (_this->m_State == VCHIQSoundIdle);

    Msg.type = VC_AUDIO_MSG_TYPE_START;

    nResult = CVCHIQSoundBaseDevice_QueueMessage(_this, &Msg);
    if (nResult != 0)
    {
        LOG (FromVCHIQSound, LogError,
                    "Cannot start audio (%d)", nResult);

        _this->m_State = VCHIQSoundError;

        return FALSE;
    }

    _this->m_State = VCHIQSoundRunning;

    vchi_service_use (_this->m_hService);

    short usPeerVersion = 0;
    nResult = vchi_get_peer_version (_this->m_hService, &usPeerVersion);
    if (nResult != 0)
    {
        vchi_service_release (_this->m_hService);

        LOG (FromVCHIQSound, LogError,
                    "Cannot get peer version (%d)", nResult);

        _this->m_State = VCHIQSoundError;

        return FALSE;
    }

    // we need peer version 2, because we do not support bulk transfers
    if (usPeerVersion < 2)
    {
        vchi_service_release (_this->m_hService);

        LOG (FromVCHIQSound, LogError,
                    "Peer version does not match (%u)", (unsigned) usPeerVersion);

        _this->m_State = VCHIQSoundError;

        return FALSE;
    }

    _this->m_nWritePos = 0;
    _this->m_nCompletePos = 0;

    nResult = CVCHIQSoundBaseDevice_WriteChunk (_this);
    if (nResult == 0)
    {
        nResult = CVCHIQSoundBaseDevice_WriteChunk (_this);
    }

    if (nResult != 0)
    {
        vchi_service_release (_this->m_hService);

        LOG (FromVCHIQSound, LogError,
                    "Cannot write audio data (%d)", nResult);

        _this->m_State = VCHIQSoundError;

        return FALSE;
    }

    vchi_service_release (_this->m_hService);

    return TRUE;
}

boolean CVCHIQSoundBaseDevice_Cancel (CVCHIQSoundBaseDevice *_this)
{
    if (_this->m_State != VCHIQSoundRunning)
    {
        return FALSE;
    }

    _this->m_State = VCHIQSoundCancelled;
    while (_this->m_State == VCHIQSoundCancelled)
    {
        ampi_co_yield ();
    }

    assert (_this->m_State == VCHIQSoundTerminating);

    vchi_service_use (_this->m_hService);

    VC_AUDIO_MSG_T Msg;

    Msg.type = VC_AUDIO_MSG_TYPE_STOP;
    Msg.u.stop.draining = 0;

    int nResult = CVCHIQSoundBaseDevice_QueueMessage(_this, &Msg);
    if (nResult != 0)
    {
        LOG (FromVCHIQSound, LogError,
                    "Cannot stop audio (%d)", nResult);

        return FALSE;
    }

    vchi_service_release (_this->m_hService);

    _this->m_State = VCHIQSoundIdle;

    return TRUE;
}

boolean CVCHIQSoundBaseDevice_IsActive (CVCHIQSoundBaseDevice *_this)
{
    return _this->m_State >= VCHIQSoundRunning;
}

void CVCHIQSoundBaseDevice_SetControl (CVCHIQSoundBaseDevice *_this, int nVolume, enum TVCHIQSoundDestination Destination)
{
    if (!(VCHIQ_SOUND_VOLUME_MIN <= nVolume && nVolume <= VCHIQ_SOUND_VOLUME_MAX))
    {
        nVolume = VCHIQ_SOUND_VOLUME_DEFAULT;
    }

    if (Destination < VCHIQSoundDestinationUnknown)
    {
        _this->m_Destination = Destination;
    }

    VC_AUDIO_MSG_T Msg;
    Msg.type = VC_AUDIO_MSG_TYPE_CONTROL;
    Msg.u.control.dest = _this->m_Destination;
    Msg.u.control.volume = VOLUME_TO_CHIP (nVolume);

    int nResult = CVCHIQSoundBaseDevice_CallMessage (_this, &Msg);
    if (nResult != 0)
    {
        LOG (FromVCHIQSound, LogWarning,
                    "Cannot set control (%d)", nResult);
    }
}
