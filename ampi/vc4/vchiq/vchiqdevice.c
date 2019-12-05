//
// vchiqdevice.cpp
//
#include <vc4/vchiq/vchiqdevice.h>

// circle/bcm2835.h
#if RASPPI == 1
#define ARM_IO_BASE		0x20000000
#elif RASPPI <= 3
#define ARM_IO_BASE		0x3F000000
#else
#define ARM_IO_BASE		0xFE000000
#endif
#define ARM_VCHIQ_BASE		(ARM_IO_BASE + 0xB840)
#define ARM_VCHIQ_END		(ARM_VCHIQ_BASE + 0x0F)

// circle/bcm2835int.h; all models use IRQ number 66
#define ARM_IRQ_ARM_DOORBELL_0	66

#include <linux/linuxemu.h>
#include <linux/assert.h>
#include <string.h>

#include <ampienv.h>

int vchiq_probe (struct platform_device *pdev);

static inline void CVCHIQDevice_AddResource (
    CVCHIQDevice *_this, u32 nStart, u32 nEnd, unsigned nFlags)
{
    assert (nStart <= nEnd);
    assert (nFlags != 0);

    unsigned i = _this->m_PlatformDevice.num_resources++;
    assert (i < PLATFORM_DEVICE_MAX_RESOURCES);

    _this->m_PlatformDevice.resource[i].start = nStart;
    _this->m_PlatformDevice.resource[i].end = nEnd;
    _this->m_PlatformDevice.resource[i].flags = nFlags;
}

static inline void CVCHIQDevice_SetDMAMemory (
    CVCHIQDevice *_this, uintptr nStart, uintptr nEnd)
{
    assert (nStart <= nEnd);

    _this->m_PlatformDevice.dev.dma_mem.start = nStart;
    _this->m_PlatformDevice.dev.dma_mem.end = nEnd;
    _this->m_PlatformDevice.dev.dma_mem.flags = IORESOURCE_DMA;
}

boolean CVCHIQDevice_Initialize (CVCHIQDevice *_this)
{
    // CVCHIQDevice::CVCHIQDevice()
    memset (&_this->m_PlatformDevice, 0, sizeof _this->m_PlatformDevice);

    _this->m_PlatformDevice.num_resources = 0;

    CVCHIQDevice_AddResource (_this, ARM_VCHIQ_BASE, ARM_VCHIQ_END, IORESOURCE_MEM);
    CVCHIQDevice_AddResource (_this, ARM_IRQ_ARM_DOORBELL_0, ARM_IRQ_ARM_DOORBELL_0, IORESOURCE_IRQ);

    uintptr start = (uintptr) GetCoherentRegion512K ();
    CVCHIQDevice_SetDMAMemory (_this, start, start + 524287);

    // CVCHIQDevice::Initialize()
    if (linuxemu_init () != 0)
    {
        return FALSE;
    }

    return vchiq_probe (&_this->m_PlatformDevice) == 0 ? TRUE : FALSE;
}
