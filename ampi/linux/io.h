#ifndef _linux_io_h
#define _linux_io_h

#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline u32 readl (const volatile void __iomem *addr)
{
	return *(u32 volatile *) ((uintptr) addr);
}


static inline void writel (u32 val, volatile void __iomem *addr)
{
	*(u32 volatile *) ((uintptr) addr) = val;
}

#ifdef __cplusplus
}
#endif

#endif
