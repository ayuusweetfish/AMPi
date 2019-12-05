//
// vchiqdevice.h
//
#ifndef _vc4_vchiq_vchiqdevice_h
#define _vc4_vchiq_vchiqdevice_h

#include <linux/platform_device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CVCHIQDevice_s
{
    struct platform_device m_PlatformDevice;
} CVCHIQDevice;

boolean CVCHIQDevice_Initialize (CVCHIQDevice *_this);

#ifdef __cplusplus
}
#endif

#endif
