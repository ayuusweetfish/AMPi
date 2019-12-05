//
// vchiqsoundbasedevice.h
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
#ifndef _vc4_sound_vchiqsoundbasedevice_h
#define _vc4_sound_vchiqsoundbasedevice_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <vc4/vchiq/vchiqdevice.h>
#include <vc4/vchi/vchi.h>
#include "vc_vchi_audioserv_defs.h"

#define VCHIQ_SOUND_VOLUME_MIN        -10000
#define VCHIQ_SOUND_VOLUME_DEFAULT    0
#define VCHIQ_SOUND_VOLUME_MAX        400

enum TVCHIQSoundDestination
{
    VCHIQSoundDestinationAuto,
    VCHIQSoundDestinationHeadphones,
    VCHIQSoundDestinationHDMI,
    VCHIQSoundDestinationUnknown
};

enum TVCHIQSoundState
{
    VCHIQSoundCreated,
    VCHIQSoundIdle,
    VCHIQSoundRunning,
    VCHIQSoundCancelled,
    VCHIQSoundTerminating,
    VCHIQSoundError,
    VCHIQSoundUnknown
};

typedef unsigned (*chunk_cb_t) (int16_t *pBuffer, unsigned nChunkSize);

typedef struct CVCHIQSoundBaseDevice_s
{
    chunk_cb_t chunk_cb;

    unsigned m_nSampleRate;
    unsigned m_nChunkSize;
    enum TVCHIQSoundDestination m_Destination;

    volatile enum TVCHIQSoundState m_State;

    VCHI_INSTANCE_T m_VCHIInstance;
    VCHI_SERVICE_HANDLE_T m_hService;

    volatile bool m_Event;
    int m_nResult;

    unsigned m_nWritePos;
    unsigned m_nCompletePos;
} CVCHIQSoundBaseDevice;

/// \param pVCHIQDevice    pointer to the VCHIQ interface device
/// \param nSampleRate    sample rate in Hz (44100..48000)
/// \param nChunkSize    number of samples transfered at once
/// \param Destination    the target device, the sound data is sent to\n
///            (detected automatically, if equal to VCHIQSoundDestinationAuto)
void CVCHIQSoundBaseDevice_Ctor (
    CVCHIQSoundBaseDevice *_this,
    CVCHIQDevice *pVCHIQDevice,
    unsigned nSampleRate,
    unsigned nChunkSize,
    enum TVCHIQSoundDestination Destination);

/// \brief Connects to the VCHIQ sound service and starts sending sound data
/// \return Operation successful?
boolean CVCHIQSoundBaseDevice_Start (CVCHIQSoundBaseDevice *_this);

/// \brief Stops the transmission of sound data
/// \note Cancel takes effect after a short delay
void CVCHIQSoundBaseDevice_Cancel (CVCHIQSoundBaseDevice *_this);

/// \return Is the sound data transmission running?
boolean CVCHIQSoundBaseDevice_IsActive (CVCHIQSoundBaseDevice *_this);

/// \param nVolume    Output volume to be set (-10000..400)
/// \param Destination    the target device, the sound data is sent to\n
///            (not modified, if equal to VCHIQSoundDestinationUnknown)
/// \note This method can be called, while the sound data transmission is running.
void CVCHIQSoundBaseDevice_SetControl (
    CVCHIQSoundBaseDevice *_this,
    int nVolume,
    enum TVCHIQSoundDestination Destination);

#ifdef __cplusplus
}
#endif

#endif
